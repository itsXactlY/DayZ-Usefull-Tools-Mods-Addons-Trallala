import xml.etree.ElementTree as ET
import requests
import json
import time
import re
from pathlib import Path
from typing import Dict, List, Optional
import shutil
import traceback


class OptimizedOllamaDayZBalancer:
    def __init__(self, types_xml_path: str, ollama_url: str = "http://localhost:11434"):
        self.types_xml_path = Path(types_xml_path)
        self.tree = ET.parse(self.types_xml_path)
        self.root = self.tree.getroot()
        self.ollama_url = ollama_url
        
        self.model = "llama3.1:8b"
        
        self.analysis_cache = {}
        self.checkpoint_file = 'balance_checkpoint.json'
        
        self.ensure_model_available()
    
    def ensure_model_available(self):
        """Check if the model is available, pull if necessary"""
        try:
            response = requests.get(f"{self.ollama_url}/api/tags", timeout=10)
            models = [model['name'] for model in response.json()['models']]
            
            if self.model not in models:
                print(f"Model {self.model} not found. Available models: {models}")
                if models:
                    self.model = models[0]
                    print(f"Using available model: {self.model}")
                else:
                    print("No models available. Please pull a model first:")
                    print("ollama pull llama3.1:8b")
                    exit(1)
            else:
                print(f"Using model: {self.model}")
                
        except Exception as e:
            print(f"Error checking models: {e}")
            print("Make sure Ollama is running: 'ollama serve'")
            exit(1)
    
    def query_ollama_single(self, prompt: str, max_retries: int = 3) -> str:
        """Query Ollama with optimized settings for single weapon analysis"""
        
        for attempt in range(max_retries):
            payload = {
                "model": self.model,
                "prompt": prompt,
                "stream": False,
                "options": {
                    "temperature": 0.1,
                    "top_k": 10,
                    "top_p": 0.9,
                    "num_ctx": 2048,  # Reduced for faster processing
                    "num_threads": 4,
                }
            }
            
            try:
                response = requests.post(f"{self.ollama_url}/api/generate", 
                                       json=payload, 
                                       timeout=30)  # Reduced timeout
                
                if response.status_code == 200:
                    result = response.json()['response']
                    if result.strip():
                        return result
                    else:
                        print("    Empty response, retrying...")
                        continue
                else:
                    print(f"    HTTP error {response.status_code}, retrying...")
                    time.sleep(1)
                    continue
                    
            except requests.exceptions.Timeout:
                print(f"    Timeout on attempt {attempt + 1}, retrying...")
                time.sleep(1)
                continue
            except Exception as e:
                print(f"    Error on attempt {attempt + 1}: {e}")
                time.sleep(1)
                continue
        
        return None
    
    def extract_json_from_response(self, response: str, weapon_name: str) -> Optional[Dict]:
        """More robust JSON extraction with multiple fallback strategies"""
        
        # Strategy 1: Look for complete JSON blocks with different patterns
        json_patterns = [
            r'\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}',  # Nested JSON
            r'\{.*?\}',  # Simple JSON
            r'\{[\s\S]*?\}',  # JSON with newlines
        ]
        
        for pattern in json_patterns:
            matches = re.findall(pattern, response, re.DOTALL)
            for match in matches:
                try:
                    # Clean up common JSON formatting issues
                    cleaned = re.sub(r',\s*}', '}', match)  # Remove trailing comma
                    cleaned = re.sub(r',\s*]', ']', cleaned)  # Remove trailing comma in arrays
                    
                    result = json.loads(cleaned)
                    if self.validate_analysis_structure(result):
                        return result
                except json.JSONDecodeError:
                    continue
        
        # Strategy 2: Line-by-line parsing for malformed JSON
        try:
            lines = response.split('\n')
            json_obj = {}
            
            for line in lines:
                line = line.strip()
                if ':' in line and any(key in line.lower() for key in ['tier', 'nominal', 'min', 'caliber', 'weapon_type', 'reasoning']):
                    # Handle different quote styles and formatting
                    if line.startswith('"') or line.startswith("'"):
                        parts = line.split(':', 1)
                    else:
                        parts = line.split(':', 1)
                    
                    if len(parts) == 2:
                        key = parts[0].strip(' "\'{}')
                        value = parts[1].strip(' "\':{},')
                        
                        # Convert numeric values
                        if key.lower() in ['tier', 'nominal', 'min']:
                            try:
                                json_obj[key.lower()] = int(re.search(r'\d+', value).group()) if re.search(r'\d+', value) else 0
                            except:
                                json_obj[key.lower()] = 0
                        else:
                            json_obj[key.lower()] = value
            
            if self.validate_analysis_structure(json_obj):
                return json_obj
                
        except Exception as e:
            print(f"    Line-by-line parsing failed: {e}")
        
        # Strategy 3: Extract key information with regex
        try:
            tier_match = re.search(r'tier["\']?\s*:\s*(\d+)', response, re.IGNORECASE)
            nominal_match = re.search(r'nominal["\']?\s*:\s*(\d+)', response, re.IGNORECASE)
            min_match = re.search(r'min["\']?\s*:\s*(\d+)', response, re.IGNORECASE)
            
            if tier_match and nominal_match and min_match:
                result = {
                    'tier': int(tier_match.group(1)),
                    'nominal': int(nominal_match.group(1)),
                    'min': int(min_match.group(1)),
                    'caliber': 'Auto-detected',
                    'weapon_type': 'auto',
                    'reasoning': 'Regex extraction'
                }
                
                if self.validate_analysis_structure(result):
                    return result
        except:
            pass
        
        return None
    
    def validate_analysis_structure(self, analysis: Dict) -> bool:
        """Validate analysis results structure and values"""
        required_keys = ['tier', 'nominal', 'min']
        
        if not all(key in analysis for key in required_keys):
            return False
            
        try:
            tier = int(analysis['tier'])
            nominal = int(analysis['nominal'])
            min_val = int(analysis['min'])
            
            if not (1 <= tier <= 5):
                return False
                
            if not (1 <= nominal <= 50):
                return False
                
            if not (0 <= min_val <= 20):
                return False
                
            return True
        except (ValueError, TypeError):
            return False
    
    def analyze_single_weapon(self, weapon_name: str, current_data: Dict) -> Dict:
        """Analyze a single weapon with enhanced prompt and robust parsing"""
        
        if weapon_name in self.analysis_cache:
            return self.analysis_cache[weapon_name]
        
        prompt = f"""You are a DayZ server administrator balancing weapon spawns.

WEAPON: {weapon_name}
CURRENT: Nominal={current_data['nominal']}, Min={current_data['min']}, Tiers={current_data['tiers']}

CLASSIFICATION RULES:
- Tier 1 (Civilian): Common weapons, shotguns, basic pistols - Nominal 15-30
- Tier 2 (Police): Police weapons, SMGs, better pistols - Nominal 8-15  
- Tier 3 (Military): Standard military rifles (AK74, M4A1) - Nominal 4-8
- Tier 4 (High-end): Rare military, sniper rifles, DMRs - Nominal 2-4
- Tier 5 (Extreme): Anti-material rifles (.50 BMG, .338) - Nominal 1-2

CALIBER CLASSIFICATION:
- .22, .380, 9mm, .45 ACP = Tier 1-2
- 5.56 NATO, 5.45x39 = Tier 2-3
- 7.62x39, .308 = Tier 3-4
- 7.62x54R, .300 Win Mag = Tier 4
- .338, .408, .50 BMG = Tier 5

EXAMPLES:
- "AK74" (5.45x39) = {{"tier":3,"nominal":6,"min":2}}
- "AKM" (7.62x39) = {{"tier":3,"nominal":5,"min":2}}
- "M4A1" (5.56) = {{"tier":3,"nominal":6,"min":2}}
- "SVD" (7.62x54R) = {{"tier":4,"nominal":3,"min":1}}

RESPOND WITH ONLY THIS EXACT JSON FORMAT:
{{"tier":3,"nominal":6,"min":2,"caliber":"5.45x39","weapon_type":"rifle","reasoning":"Standard military rifle"}}

JSON for {weapon_name}:"""

        response = self.query_ollama_single(prompt)
        
        if response:
            analysis = self.extract_json_from_response(response, weapon_name)
            if analysis:
                # Ensure all required fields exist
                analysis.setdefault('caliber', 'Unknown')
                analysis.setdefault('weapon_type', 'unknown')
                analysis.setdefault('reasoning', 'AI analysis')
                
                self.analysis_cache[weapon_name] = analysis
                return analysis
            else:
                print(f"    Failed to extract valid JSON for {weapon_name}")
        
        return self.fallback_analysis(weapon_name)
    
    def fallback_analysis(self, weapon_name: str) -> Dict:
        """Enhanced fallback analysis based on weapon name patterns"""
        name_lower = weapon_name.lower()
        
        # Enhanced pattern matching
        patterns = [
            # Extreme tier (Tier 5)
            (['338', '408', '50cal', 'bmg', 'cheytac', 'lynx'], {
                "tier": 5, "nominal": 1, "min": 0,
                "caliber": "High-power rifle", "weapon_type": "sniper",
                "reasoning": "Extremely rare anti-material rifle"
            }),
            
            # High-end military (Tier 4)
            (['svd', 'dragunov', 'vss', 'val', 'sr25', 'mk17', 'scar'], {
                "tier": 4, "nominal": 3, "min": 1,
                "caliber": "Precision rifle", "weapon_type": "sniper",
                "reasoning": "Rare precision/sniper rifle"
            }),
            
            # Military rifle calibers (Tier 3)
            (['762', '308', '300', 'nato', 'akm', 'ak47', 'fal', 'g3'], {
                "tier": 3, "nominal": 5, "min": 2,
                "caliber": "Military rifle", "weapon_type": "rifle",
                "reasoning": "Military-grade battle rifle"
            }),
            
            # Standard military (Tier 3)
            (['556', '223', '545', 'ak74', 'ak12', 'm4', 'm16', 'aug'], {
                "tier": 3, "nominal": 6, "min": 2,
                "caliber": "Standard military", "weapon_type": "rifle",
                "reasoning": "Standard military assault rifle"
            }),
            
            # Suppressors and attachments (Tier 4)
            (['sup', 'silencer', 'suppressor', 'compensator', 'muzzle'], {
                "tier": 4, "nominal": 3, "min": 1,
                "caliber": "Attachment", "weapon_type": "attachment",
                "reasoning": "Rare weapon attachment"
            }),
            
            # SMGs and PDWs (Tier 2)
            (['smg', 'ump', 'mp5', 'pp19', 'bizon', 'kriss', 'vector'], {
                "tier": 2, "nominal": 10, "min": 4,
                "caliber": "SMG", "weapon_type": "smg",
                "reasoning": "Police/military SMG"
            }),
            
            # Pistol calibers (Tier 2)
            (['9mm', '45', '380', 'pistol', 'glock', 'colt', 'sig', 'beretta'], {
                "tier": 2, "nominal": 12, "min": 5,
                "caliber": "Pistol", "weapon_type": "pistol",
                "reasoning": "Military/police sidearm"
            }),
            
            # Shotguns (Tier 1-2)
            (['shotgun', 'izh', 'mosin', 'double', 'pump'], {
                "tier": 1, "nominal": 15, "min": 6,
                "caliber": "Shotgun", "weapon_type": "shotgun",
                "reasoning": "Civilian hunting weapon"
            }),
        ]
        
        # Check patterns in order of priority
        for pattern_list, result in patterns:
            if any(pattern in name_lower for pattern in pattern_list):
                return result
        
        # Default for completely unknown weapons
        return {
            "tier": 2, "nominal": 8, "min": 3,
            "caliber": "Unknown", "weapon_type": "unknown",
            "reasoning": "Default classification - unknown weapon type"
        }
    
    def collect_weapon_items(self, filter_pattern: str = None) -> List[ET.Element]:
        """Collect all weapon items with optional filtering"""
        weapon_items = []
        
        for item in self.root.findall('type'):
            name = item.get('name')
            
            if filter_pattern and filter_pattern not in name:
                continue
            
            category = item.find('category')
            if category is None or category.get('name') != 'weapons':
                continue
                
            weapon_items.append(item)
        
        return weapon_items
    
    def extract_current_data(self, item: ET.Element) -> Dict:
        """Extract current weapon data from XML element"""
        return {
            'nominal': item.find('nominal').text if item.find('nominal') is not None else "0",
            'min': item.find('min').text if item.find('min') is not None else "0",
            'tiers': [elem.get('name') for elem in item.findall('value') if elem.get('name', '').startswith('Tier')]
        }
    
    def save_checkpoint(self, changes: List[Dict], processed_count: int):
        """Save progress checkpoint"""
        checkpoint = {
            'processed_count': processed_count,
            'changes': changes,
            'timestamp': time.time(),
            'cache': self.analysis_cache
        }
        
        try:
            with open(self.checkpoint_file, 'w', encoding='utf-8') as f:
                json.dump(checkpoint, f, indent=2, ensure_ascii=False)
            print(f"    Checkpoint saved at {processed_count} weapons")
        except Exception as e:
            print(f"    Failed to save checkpoint: {e}")
    
    def load_checkpoint(self) -> Optional[Dict]:
        """Load previous checkpoint if available"""
        checkpoint_path = Path(self.checkpoint_file)
        if checkpoint_path.exists():
            try:
                with open(checkpoint_path, encoding='utf-8') as f:
                    checkpoint = json.load(f)
                    # Restore analysis cache
                    if 'cache' in checkpoint:
                        self.analysis_cache = checkpoint['cache']
                    return checkpoint
            except Exception as e:
                print(f"Failed to load checkpoint: {e}")
        return None
    
    def process_weapons_with_progress(self, filter_pattern: str = None, resume: bool = True) -> List[Dict]:
        """Process weapons with progress tracking, error recovery, and resume capability"""
        weapon_items = self.collect_weapon_items(filter_pattern)
        
        print(f"Found {len(weapon_items)} weapons to process")
        
        all_changes = []
        failed_weapons = []
        start_index = 0
        
        # Check for resume capability
        if resume:
            checkpoint = self.load_checkpoint()
            if checkpoint and input(f"Resume from checkpoint at {checkpoint['processed_count']} weapons? (y/n): ").lower() == 'y':
                all_changes = checkpoint['changes']
                start_index = checkpoint['processed_count']
                print(f"Resuming from weapon {start_index + 1}")
        
        for i in range(start_index, len(weapon_items)):
            item = weapon_items[i]
            name = item.get('name')
            progress = f"[{i+1}/{len(weapon_items)}]"
            
            print(f"{progress} Processing: {name}")
            
            try:
                current_data = self.extract_current_data(item)
                analysis = self.analyze_single_weapon(name, current_data)
                
                if analysis and self.validate_analysis_structure(analysis):
                    changes = self.apply_weapon_analysis(item, analysis)
                    all_changes.append(changes)
                    
                    # Show progress with color coding
                    tier_symbol = "🔴" if analysis['tier'] >= 4 else "🟡" if analysis['tier'] == 3 else "🟢"
                    print(f"  ✓ {tier_symbol} Tier {analysis['tier']}, Nominal {analysis['nominal']} - {analysis['reasoning'][:50]}...")
                else:
                    failed_weapons.append(name)
                    print(f"  ⚠ Analysis failed, using fallback")
                    fallback = self.fallback_analysis(name)
                    changes = self.apply_weapon_analysis(item, fallback)
                    all_changes.append(changes)
                    
                # Progress checkpoint every 10 weapons
                if (i + 1) % 10 == 0:
                    self.save_checkpoint(all_changes, i + 1)
                    
            except KeyboardInterrupt:
                print(f"\n⚠ Process interrupted by user at weapon {i+1}")
                self.save_checkpoint(all_changes, i)
                raise
            except Exception as e:
                print(f"  ✗ Error processing {name}: {e}")
                failed_weapons.append(name)
                continue
                
            time.sleep(0.3)  # Reduced delay for faster processing
        
        # Clean up checkpoint file on completion
        if Path(self.checkpoint_file).exists():
            Path(self.checkpoint_file).unlink()
        
        print(f"\n🎉 Processing Complete!")
        print(f"✅ Successfully processed: {len(all_changes) - len(failed_weapons)} weapons")
        if failed_weapons:
            print(f"⚠ Failed weapons: {len(failed_weapons)} - {', '.join(failed_weapons[:5])}{'...' if len(failed_weapons) > 5 else ''}")
        
        return all_changes
    
    def apply_weapon_analysis(self, item: ET.Element, analysis: Dict) -> Dict:
        """Apply the AI analysis to a weapon item with enhanced logic"""
        name = item.get('name')
        
        original = {
            'nominal': item.find('nominal').text if item.find('nominal') is not None else "0",
            'min': item.find('min').text if item.find('min') is not None else "0",
            'tiers': [elem.get('name') for elem in item.findall('value') if elem.get('name', '').startswith('Tier')]
        }
        
        # Apply new values
        item.find('nominal').text = str(analysis['nominal'])
        item.find('min').text = str(analysis['min'])
        
        # Enhanced restock times based on tier
        restock_times = {1: 3600, 2: 5400, 3: 10800, 4: 18000, 5: 28800}
        new_restock = restock_times.get(analysis['tier'], 10800)
        
        if item.find('restock') is not None:
            item.find('restock').text = str(new_restock)
        
        # Remove old tier assignments
        for tier_elem in item.findall('value'):
            tier_name = tier_elem.get('name')
            if tier_name and tier_name.startswith('Tier'):
                item.remove(tier_elem)
        
        # Add new tier assignment
        correct_tier = f"Tier{analysis['tier']}"
        new_tier = ET.SubElement(item, 'value')
        new_tier.set('name', correct_tier)
        
        # Enhanced location mapping
        location_mapping = {
            1: ["Town", "Village"],
            2: ["Town", "Village", "Police"],
            3: ["Military", "Police"],
            4: ["Military"],
            5: ["Military"]
        }

        # Remove old usage assignments
        for usage_elem in item.findall('usage'):
            item.remove(usage_elem)

        # Add appropriate usage locations
        for location in location_mapping.get(analysis['tier'], ["Military"]):
            usage_elem = ET.SubElement(item, 'usage')
            usage_elem.set('name', location)
        
        return {
            'name': name,
            'original': original,
            'analysis': analysis,
            'applied': True
        }
    
    def generate_enhanced_report(self, changes: List[Dict]) -> str:
        """Generate a comprehensive report with statistics and analysis"""
        report = "🎯 OLLAMA DAYZ WEAPON BALANCE REPORT\n"
        report += "=" * 60 + "\n\n"
        
        successful = [c for c in changes if c['applied']]
        report += f"📊 SUMMARY:\n"
        report += f"   Total Processed: {len(successful)} weapons\n"
        report += f"   Model Used: {self.model}\n"
        report += f"   Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n"
        
        # Tier distribution analysis
        tier_counts = {}
        tier_changes = {}
        nominal_changes = []
        
        for change in successful:
            tier = change['analysis']['tier']
            tier_counts[tier] = tier_counts.get(tier, 0) + 1
            
            # Track tier changes
            old_tiers = change['original']['tiers']
            old_tier_num = int(old_tiers[0].replace('Tier', '')) if old_tiers else 0
            if old_tier_num != tier:
                tier_changes[change['name']] = (old_tier_num, tier)
            
            # Track nominal changes
            old_nominal = int(change['original']['nominal'])
            new_nominal = change['analysis']['nominal']
            if old_nominal != new_nominal:
                nominal_changes.append((change['name'], old_nominal, new_nominal))
        
        report += "📈 TIER DISTRIBUTION:\n"
        for tier in sorted(tier_counts.keys()):
            tier_name = {1: "Civilian", 2: "Police", 3: "Military", 4: "High-end", 5: "Extreme"}[tier]
            percentage = (tier_counts[tier] / len(successful)) * 100
            bar = "█" * int(percentage / 5)
            report += f"   Tier {tier} ({tier_name}): {tier_counts[tier]} weapons ({percentage:.1f}%) {bar}\n"
        report += "\n"
        
        # Significant changes
        if tier_changes:
            report += f"🔄 TIER CHANGES ({len(tier_changes)} weapons):\n"
            for name, (old, new) in list(tier_changes.items())[:10]:
                direction = "↑" if new > old else "↓"
                report += f"   {name}: Tier {old} → {new} {direction}\n"
            if len(tier_changes) > 10:
                report += f"   ... and {len(tier_changes) - 10} more\n"
            report += "\n"
        
        # Weapon type analysis
        weapon_types = {}
        caliber_types = {}
        for change in successful:
            wtype = change['analysis'].get('weapon_type', 'unknown')
            caliber = change['analysis'].get('caliber', 'unknown')
            weapon_types[wtype] = weapon_types.get(wtype, 0) + 1
            caliber_types[caliber] = caliber_types.get(caliber, 0) + 1
        
        report += "🔫 WEAPON TYPE BREAKDOWN:\n"
        for wtype, count in sorted(weapon_types.items(), key=lambda x: x[1], reverse=True):
            report += f"   {wtype.title()}: {count} weapons\n"
        report += "\n"
        
        # Detailed weapon listing
        report += "📋 DETAILED WEAPON ANALYSIS:\n"
        report += "-" * 60 + "\n"
        
        # Sort by tier then by name
        sorted_changes = sorted(successful, key=lambda x: (x['analysis']['tier'], x['name']))
        
        current_tier = None
        for change in sorted_changes:
            analysis = change['analysis']
            original = change['original']
            
            if analysis['tier'] != current_tier:
                current_tier = analysis['tier']
                tier_name = {1: "CIVILIAN", 2: "POLICE", 3: "MILITARY", 4: "HIGH-END", 5: "EXTREME"}[current_tier]
                report += f"\n🏷️  TIER {current_tier} - {tier_name}:\n"
            
            # Show changes with indicators
            nominal_change = ""
            if int(original['nominal']) != analysis['nominal']:
                old_n = int(original['nominal'])
                new_n = analysis['nominal']
                change_indicator = "↑" if new_n > old_n else "↓"
                nominal_change = f" ({old_n}→{new_n}{change_indicator})"
            
            report += f"   {change['name']:<25} | "
            report += f"Nom: {analysis['nominal']:>2}{nominal_change:<8} | "
            report += f"Min: {analysis['min']:>2} | "
            report += f"{analysis['weapon_type']:<10} | "
            report += f"{analysis['caliber']:<15}\n"
            
            if len(analysis.get('reasoning', '')) > 0:
                report += f"     └─ {analysis['reasoning']}\n"
        
        return report
    
    def save_balanced_xml(self, output_path: Optional[str] = None) -> Path:
        """Save the balanced XML with backup"""
        if output_path is None:
            timestamp = int(time.time())
            output_path = self.types_xml_path.parent / f'types_balanced_{timestamp}.xml'
        
        output_path = Path(output_path)
        
        # Create backup
        backup_path = self.types_xml_path.parent / f'types_backup_{int(time.time())}.xml'
        shutil.copy2(self.types_xml_path, backup_path)
        print(f"📁 Backup created: {backup_path}")
        
        # Save balanced version
        self.tree.write(output_path, encoding='utf-8', xml_declaration=True)
        print(f"💾 Balanced XML saved: {output_path}")
        
        return output_path


def main():
    """Enhanced main execution with better error handling and user experience"""
    print("🎮 DayZ Weapon Balancer with Ollama AI")
    print("=" * 40)
    
    try:
        # Test Ollama connection
        print("🔍 Testing Ollama connection...")
        test_response = requests.get("http://localhost:11434/api/tags", timeout=5)
        if test_response.status_code != 200:
            print("❌ Ollama is not responding properly. Make sure it's running:")
            print("   ollama serve")
            return
        
        print("✅ Ollama connection OK!")
        
        # Initialize balancer
        xml_file = input("📁 Enter XML file path (or press Enter for 'docs_weapons.xml'): ").strip()
        if not xml_file:
            xml_file = 'docs_weapons.xml'
        
        if not Path(xml_file).exists():
            print(f"❌ File not found: {xml_file}")
            return
        
        balancer = OptimizedOllamaDayZBalancer(xml_file)
        
        # Get filter options
        filter_mod = input("🔍 Filter by mod name (e.g., 'Doc_' or press Enter for all): ").strip()
        filter_pattern = filter_mod if filter_mod else None
        
        if filter_pattern:
            preview_items = balancer.collect_weapon_items(filter_pattern)
            print(f"📊 Found {len(preview_items)} weapons matching '{filter_pattern}'")
            if len(preview_items) == 0:
                print("❌ No weapons found with that filter. Exiting.")
                return
        
        # Confirm processing
        print(f"\n🚀 Ready to process with model: {balancer.model}")
        if input("Continue? (y/n): ").lower() != 'y':
            return
        
        print("\n" + "="*50)
        print("🔄 PROCESSING WEAPONS...")
        print("="*50)
        
        # Process weapons
        changes = balancer.process_weapons_with_progress(filter_pattern=filter_pattern)
        
        # Generate report
        print("\n📊 Generating report...")
        report = balancer.generate_enhanced_report(changes)
        
        # Save report
        timestamp = int(time.time())
        report_path = f'balance_report_{timestamp}.txt'
        with open(report_path, 'w', encoding='utf-8') as f:
            f.write(report)
        
        print(f"📄 Report saved to: {report_path}")
        
        # Preview report
        print("\n" + "="*50)
        print("📋 BALANCE REPORT PREVIEW:")
        print("="*50)
        report_lines = report.split('\n')
        for line in report_lines[:30]:  # Show first 30 lines
            print(line)
        if len(report_lines) > 30:
            print(f"\n... and {len(report_lines) - 30} more lines in full report")
        
        # Save XML option
        print(f"\n💾 Processing complete! {len(changes)} weapons processed.")
        if input("Save balanced XML file? (y/n): ").lower() == 'y':
            output_file = balancer.save_balanced_xml()
            print(f"✅ Balanced XML saved to: {output_file}")
           
        # Offer to open report
        if input("📖 Open report file? (y/n): ").lower() == 'y':
            import os
            import subprocess
            import platform
            
            try:
                if platform.system() == 'Windows':
                    os.startfile(report_path)
                elif platform.system() == 'Darwin':  # macOS
                    subprocess.run(['open', report_path])
                else:  # Linux
                    subprocess.run(['xdg-open', report_path])
            except Exception as e:
                print(f"Could not open file automatically: {e}")
       
        print("\n🎉 All done! Thanks for using DayZ Weapon Balancer!")
       
    except KeyboardInterrupt:
        print("\n⚠️ Process interrupted by user")
        print("💡 You can resume processing later if a checkpoint was saved.")
    except FileNotFoundError as e:
        print(f"❌ File not found: {e}")
    except ET.ParseError as e:
        print(f"❌ XML parsing error: {e}")
        print("💡 Make sure your XML file is valid and well-formed.")
    except requests.exceptions.ConnectionError:
        print("❌ Cannot connect to Ollama. Make sure it's running:")
        print("   ollama serve")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        print("\n🔍 Full error details:")
        traceback.print_exc()
        print("\n💡 Please report this error if it persists.")


if __name__ == "__main__":
   main()

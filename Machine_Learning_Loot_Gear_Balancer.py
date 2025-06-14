import xml.etree.ElementTree as ET
import requests
import json
import time
import re
from pathlib import Path
from typing import Dict, List, Optional
import shutil
import traceback


class OptimizedOllamaGearBalancer:
    def __init__(self, types_xml_path: str, ollama_url: str = "http://localhost:11434"):
        self.types_xml_path = Path(types_xml_path)
        self.tree = ET.parse(self.types_xml_path)
        self.root = self.tree.getroot()
        self.ollama_url = ollama_url
        
        self.model = "llama3.1:8b"
        
        self.analysis_cache = {}
        self.checkpoint_file = 'gear_balance_checkpoint.json'
        
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
        """Query Ollama with optimized settings for single gear analysis"""
        
        for attempt in range(max_retries):
            payload = {
                "model": self.model,
                "prompt": prompt,
                "stream": False,
                "options": {
                    "temperature": 0.1,
                    "top_k": 10,
                    "top_p": 0.9,
                    "num_ctx": 2048,
                    "num_threads": 4,
                }
            }
            
            try:
                response = requests.post(f"{self.ollama_url}/api/generate", 
                                       json=payload, 
                                       timeout=30)
                
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
    
    def extract_json_from_response(self, response: str, item_name: str) -> Optional[Dict]:
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
                if ':' in line and any(key in line.lower() for key in ['tier', 'nominal', 'min', 'item_type', 'category', 'reasoning']):
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
                    'item_type': 'Auto-detected',
                    'category': 'auto',
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
                
            if not (1 <= nominal <= 100):  # Gear can have higher spawn rates
                return False
                
            if not (0 <= min_val <= 30):  # Gear can have higher minimums
                return False
                
            return True
        except (ValueError, TypeError):
            return False
    
    def analyze_single_gear(self, item_name: str, current_data: Dict) -> Dict:
        """Analyze a single gear item with enhanced prompt for gear/equipment"""
        
        if item_name in self.analysis_cache:
            return self.analysis_cache[item_name]
        
        prompt = f"""You are a DayZ server administrator balancing gear and equipment spawns.

ITEM: {item_name}
CURRENT: Nominal={current_data['nominal']}, Min={current_data['min']}, Tiers={current_data['tiers']}, Category={current_data['category']}

GEAR CLASSIFICATION RULES:
- Tier 1 (Civilian): Common clothes, basic backpacks, civilian gear - Nominal 20-50
- Tier 2 (Police): Police gear, tactical clothes, police backpacks - Nominal 10-25  
- Tier 3 (Military): Military clothes, combat gear, tactical backpacks - Nominal 5-15
- Tier 4 (Special): Elite military gear, rare equipment, high-end backpacks - Nominal 2-8
- Tier 5 (Extreme): Ultra-rare gear, experimental equipment - Nominal 1-3

ITEM TYPE CLASSIFICATION:
- Backpacks: Storage capacity determines tier (larger = higher tier)
  * Small bags (school, canvas) = Tier 1-2, Nominal 15-30
  * Medium backpacks (hiking, field) = Tier 2-3, Nominal 8-18  
  * Large backpacks (assault, mountain) = Tier 3-4, Nominal 3-10
  * Tactical/Military packs = Tier 4-5, Nominal 1-5

- Clothing: Protection and rarity determines tier
  * Civilian clothes = Tier 1, Nominal 25-50
  * Work/utility clothes = Tier 2, Nominal 15-30
  * Tactical/Police gear = Tier 3, Nominal 8-15
  * Military uniforms = Tier 4, Nominal 3-8
  * Special forces gear = Tier 5, Nominal 1-3

- Equipment/Tools: Functionality and rarity
  * Basic tools = Tier 1-2, Nominal 20-40
  * Professional equipment = Tier 3, Nominal 8-15
  * Military equipment = Tier 4-5, Nominal 2-8

- Storage/Containers: Size and protection level
  * Basic containers = Tier 1-2, Nominal 15-25
  * Protective cases = Tier 3-4, Nominal 5-12
  * Military storage = Tier 4-5, Nominal 2-5

EXAMPLES:
- "SchoolBackpack" = {{"tier":1,"nominal":25,"min":8}}
- "AssaultBackpack" = {{"tier":4,"nominal":4,"min":1}}
- "TacticalVest_Black" = {{"tier":3,"nominal":8,"min":3}}
- "PolicePants" = {{"tier":2,"nominal":15,"min":5}}
- "GhillieSuit_Woodland" = {{"tier":5,"nominal":2,"min":0}}

RESPOND WITH ONLY THIS EXACT JSON FORMAT:
{{"tier":3,"nominal":8,"min":3,"item_type":"backpack","category":"equipment","reasoning":"Military grade equipment"}}

JSON for {item_name}:"""

        response = self.query_ollama_single(prompt)
        
        if response:
            analysis = self.extract_json_from_response(response, item_name)
            if analysis:
                # Ensure all required fields exist
                analysis.setdefault('item_type', 'unknown')
                analysis.setdefault('category', current_data.get('category', 'equipment'))
                analysis.setdefault('reasoning', 'AI analysis')
                
                self.analysis_cache[item_name] = analysis
                return analysis
            else:
                print(f"    Failed to extract valid JSON for {item_name}")
        
        return self.fallback_analysis(item_name, current_data)
    
    def fallback_analysis(self, item_name: str, current_data: Dict) -> Dict:
        """Enhanced fallback analysis based on item name patterns and categories"""
        name_lower = item_name.lower()
        category = current_data.get('category', '').lower()
        
        # Enhanced pattern matching for gear
        patterns = [
            # Backpacks - Size-based tiers
            (['assault', 'combat', 'tactical', 'mountain'], {
                "tier": 4, "nominal": 4, "min": 1,
                "item_type": "backpack", "category": "equipment",
                "reasoning": "Large military/tactical backpack"
            }),
            
            (['field', 'hiking', 'hunter'], {
                "tier": 3, "nominal": 8, "min": 3,
                "item_type": "backpack", "category": "equipment",
                "reasoning": "Medium outdoor backpack"
            }),
            
            (['school', 'canvas', 'sling'], {
                "tier": 1, "nominal": 20, "min": 8,
                "item_type": "backpack", "category": "equipment",
                "reasoning": "Small civilian backpack"
            }),
            
            # Military clothing and gear
            (['ghillie', 'sniper', 'special'], {
                "tier": 5, "nominal": 2, "min": 0,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Ultra-rare specialized gear"
            }),
            
            (['military', 'combat', 'tactical', 'army'], {
                "tier": 4, "nominal": 6, "min": 2,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Military-grade equipment"
            }),
            
            # Police gear
            (['police', 'cop', 'officer'], {
                "tier": 2, "nominal": 12, "min": 4,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Police equipment"
            }),
            
            # Vests and protective gear
            (['vest', 'armor', 'plate', 'carrier'], {
                "tier": 3, "nominal": 8, "min": 3,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Protective equipment"
            }),
            
            # Helmets
            (['helmet', 'ballistic'], {
                "tier": 3, "nominal": 6, "min": 2,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Head protection"
            }),
            
            # Tools and equipment
            (['tool', 'kit', 'device'], {
                "tier": 2, "nominal": 15, "min": 5,
                "item_type": "equipment", "category": "tools",
                "reasoning": "Utility equipment"
            }),
            
            # Storage containers
            (['case', 'box', 'container', 'locker'], {
                "tier": 3, "nominal": 8, "min": 3,
                "item_type": "container", "category": "equipment",
                "reasoning": "Storage equipment"
            }),
            
            # Civilian clothing
            (['shirt', 'pants', 'jacket', 'coat', 'jeans'], {
                "tier": 1, "nominal": 30, "min": 10,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Common civilian clothing"
            }),
        ]
        
        # Check patterns in order of priority
        for pattern_list, result in patterns:
            if any(pattern in name_lower for pattern in pattern_list):
                return result
        
        # Category-based fallbacks
        if category == 'clothes':
            return {
                "tier": 2, "nominal": 15, "min": 5,
                "item_type": "clothing", "category": "clothes",
                "reasoning": "Default clothing classification"
            }
        elif 'backpack' in name_lower or 'bag' in name_lower:
            return {
                "tier": 2, "nominal": 12, "min": 4,
                "item_type": "backpack", "category": "equipment",
                "reasoning": "Default backpack classification"
            }
        elif category == 'containers':
            return {
                "tier": 3, "nominal": 8, "min": 3,
                "item_type": "container", "category": "containers",
                "reasoning": "Default container classification"
            }
        
        # Default for completely unknown gear
        return {
            "tier": 2, "nominal": 10, "min": 3,
            "item_type": "unknown", "category": category or "equipment",
            "reasoning": "Default classification - unknown gear type"
        }
    
    def collect_gear_items(self, filter_pattern: str = None) -> List[ET.Element]:
        """Collect all gear items (clothes, containers, equipment) with optional filtering"""
        gear_items = []
        gear_categories = ['clothes', 'containers', 'equipment', 'tools']
        
        for item in self.root.findall('type'):
            name = item.get('name')
            
            if filter_pattern and filter_pattern not in name:
                continue
            
            category = item.find('category')
            if category is None:
                continue
                
            category_name = category.get('name', '').lower()
            
            # Include gear categories, exclude weapons/ammo/food/medical
            if category_name in gear_categories or any(gear_cat in category_name for gear_cat in gear_categories):
                gear_items.append(item)
            elif category_name not in ['weapons', 'ammunition', 'explosives', 'food', 'medical']:
                # Include unknown categories that might be gear
                gear_items.append(item)
        
        return gear_items
    
    def extract_current_data(self, item: ET.Element) -> Dict:
        """Extract current gear data from XML element"""
        category_elem = item.find('category')
        category_name = category_elem.get('name') if category_elem is not None else 'unknown'
        
        return {
            'nominal': item.find('nominal').text if item.find('nominal') is not None else "0",
            'min': item.find('min').text if item.find('min') is not None else "0",
            'tiers': [elem.get('name') for elem in item.findall('value') if elem.get('name', '').startswith('Tier')],
            'category': category_name
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
            print(f"    Checkpoint saved at {processed_count} items")
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
    
    def process_gear_with_progress(self, filter_pattern: str = None, resume: bool = True) -> List[Dict]:
        """Process gear items with progress tracking, error recovery, and resume capability"""
        gear_items = self.collect_gear_items(filter_pattern)
        
        print(f"Found {len(gear_items)} gear items to process")
        
        all_changes = []
        failed_items = []
        start_index = 0
        
        # Check for resume capability
        if resume:
            checkpoint = self.load_checkpoint()
            if checkpoint and input(f"Resume from checkpoint at {checkpoint['processed_count']} items? (y/n): ").lower() == 'y':
                all_changes = checkpoint['changes']
                start_index = checkpoint['processed_count']
                print(f"Resuming from item {start_index + 1}")
        
        for i in range(start_index, len(gear_items)):
            item = gear_items[i]
            name = item.get('name')
            progress = f"[{i+1}/{len(gear_items)}]"
            
            print(f"{progress} Processing: {name}")
            
            try:
                current_data = self.extract_current_data(item)
                analysis = self.analyze_single_gear(name, current_data)
                
                if analysis and self.validate_analysis_structure(analysis):
                    changes = self.apply_gear_analysis(item, analysis)
                    all_changes.append(changes)
                    
                    # Show progress with color coding
                    tier_symbol = "🔴" if analysis['tier'] >= 4 else "🟡" if analysis['tier'] == 3 else "🟢"
                    print(f"  ✓ {tier_symbol} Tier {analysis['tier']}, Nominal {analysis['nominal']} - {analysis['reasoning'][:50]}...")
                else:
                    failed_items.append(name)
                    print(f"  ⚠ Analysis failed, using fallback")
                    fallback = self.fallback_analysis(name, current_data)
                    changes = self.apply_gear_analysis(item, fallback)
                    all_changes.append(changes)
                    
                # Progress checkpoint every 10 items
                if (i + 1) % 10 == 0:
                    self.save_checkpoint(all_changes, i + 1)
                    
            except KeyboardInterrupt:
                print(f"\n⚠ Process interrupted by user at item {i+1}")
                self.save_checkpoint(all_changes, i)
                raise
            except Exception as e:
                print(f"  ✗ Error processing {name}: {e}")
                failed_items.append(name)
                continue
                
            time.sleep(0.3)
        
        # Clean up checkpoint file on completion
        if Path(self.checkpoint_file).exists():
            Path(self.checkpoint_file).unlink()
        
        print(f"\n🎉 Processing Complete!")
        print(f"✅ Successfully processed: {len(all_changes) - len(failed_items)} items")
        if failed_items:
            print(f"⚠ Failed items: {len(failed_items)} - {', '.join(failed_items[:5])}{'...' if len(failed_items) > 5 else ''}")
        
        return all_changes
    
    def apply_gear_analysis(self, item: ET.Element, analysis: Dict) -> Dict:
        """Apply the AI analysis to a gear item with enhanced logic"""
        name = item.get('name')
        
        original = {
            'nominal': item.find('nominal').text if item.find('nominal') is not None else "0",
            'min': item.find('min').text if item.find('min') is not None else "0",
            'tiers': [elem.get('name') for elem in item.findall('value') if elem.get('name', '').startswith('Tier')],
            'category': item.find('category').get('name') if item.find('category') is not None else 'unknown'
        }
        
        # Apply new values
        item.find('nominal').text = str(analysis['nominal'])
        item.find('min').text = str(analysis['min'])
        
        # Enhanced restock times based on tier and item type
        base_restock_times = {1: 1800, 2: 3600, 3: 7200, 4: 14400, 5: 21600}
        
        # Adjust restock based on item type
        item_type = analysis.get('item_type', 'unknown')
        multiplier = 1.0
        
        if item_type == 'backpack':
            multiplier = 1.5  # Backpacks take longer to restock
        elif item_type == 'clothing':
            multiplier = 0.8  # Clothing restocks faster
        elif item_type == 'container':
            multiplier = 1.2  # Containers moderate restock
        
        new_restock = int(base_restock_times.get(analysis['tier'], 7200) * multiplier)
        
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
        
        # Enhanced location mapping for gear
        location_mapping = {
            1: ["Town", "Village", "Farm"],  # Civilian areas
            2: ["Town", "Village", "Police", "Office"],  # Police + civilian
            3: ["Military", "Police", "Industrial"],  # Military + police
            4: ["Military", "Bunker"],  # High-end military
            5: ["Military", "Bunker"]  # Ultra-rare military
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
        """Generate a comprehensive report with statistics and analysis for gear"""
        report = "🎒 OLLAMA DAYZ GEAR BALANCE REPORT\n"
        report += "=" * 60 + "\n\n"
        
        successful = [c for c in changes if c['applied']]
        report += f"📊 SUMMARY:\n"
        report += f"   Total Processed: {len(successful)} items\n"
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
            report += f"   Tier {tier} ({tier_name}): {tier_counts[tier]} items ({percentage:.1f}%) {bar}\n"
        report += "\n"
        
        # Item type analysis
        item_types = {}
        category_types = {}
        for change in successful:
            itype = change['analysis'].get('item_type', 'unknown')
            category = change['analysis'].get('category', 'unknown')
            item_types[itype] = item_types.get(itype, 0) + 1
            category_types[category] = category_types.get(category, 0) + 1
        
        report += "🎒 ITEM TYPE BREAKDOWN:\n"
        for itype, count in sorted(item_types.items(), key=lambda x: x[1], reverse=True):
            report += f"   {itype.title()}: {count} items\n"
        report += "\n"
        
        report += "📂 CATEGORY BREAKDOWN:\n"
        for category, count in sorted(category_types.items(), key=lambda x: x[1], reverse=True):
            report += f"   {category.title()}: {count} items\n"
        report += "\n"
        
        # Significant changes
        if tier_changes:
            report += f"🔄 TIER CHANGES ({len(tier_changes)} items):\n"
            for name, (old, new) in list(tier_changes.items())[:10]:
                direction = "↑" if new > old else "↓"
                report += f"   {name}: Tier {old} → {new} {direction}\n"
            if len(tier_changes) > 10:
                report += f"   ... and {len(tier_changes) - 10} more\n"
            report += "\n"
        
        # Detailed item listing
        report += "📋 DETAILED GEAR ANALYSIS:\n"
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
            
            report += f"   {change['name']:<30} | "
            report += f"Nom: {analysis['nominal']:>2}{nominal_change:<8} | "
            report += f"Min: {analysis['min']:>2} | "
            report += f"{analysis['item_type']:<12} | "
            report += f"{analysis['category']:<12}\n"
            
            if len(analysis.get('reasoning', '')) > 0:
                report += f"     └─ {analysis['reasoning']}\n"
        
        return report
    
def save_balanced_xml(self, output_path: Optional[str] = None) -> Path:
        """Save the balanced XML with backup"""
        if output_path is None:
            timestamp = int(time.time())
            # Create output filename based on input filename
            input_name = self.types_xml_path.stem  # gets filename without extension
            output_path = self.types_xml_path.parent / f'{input_name}_gear_balanced_{timestamp}.xml'
        
        output_path = Path(output_path)
        
        # Create backup with original filename preserved
        input_name = self.types_xml_path.stem
        backup_path = self.types_xml_path.parent / f'{input_name}_backup_{int(time.time())}.xml'
        shutil.copy2(self.types_xml_path, backup_path)
        print(f"📁 Backup created: {backup_path}")
        
        # Save balanced version
        self.tree.write(output_path, encoding='utf-8', xml_declaration=True)
        print(f"💾 Balanced XML saved: {output_path}")
        
        return output_path


def process_multiple_files():
    """Process multiple types files with batch processing"""
    print("🎒 DayZ Gear Balancer - BATCH MODE")
    print("=" * 50)
    
    # Get all XML files in current directory
    xml_files = list(Path('.').glob('*.xml'))
    types_files = [f for f in xml_files if 'types' in f.name.lower() and not any(x in f.name.lower() for x in ['backup', 'balanced', 'weapons'])]
    
    if not types_files:
        print("❌ No types XML files found in current directory")
        return
    
    print("📁 Found these types files:")
    for i, file in enumerate(types_files, 1):
        print(f"   {i:2d}. {file.name}")
    
    print("\nProcessing options:")
    print("   A. Process ALL files")
    print("   S. Select specific files")
    print("   1-N. Process single file by number")
    
    choice = input("\nYour choice: ").strip().upper()
    
    selected_files = []
    
    if choice == 'A':
        selected_files = types_files
    elif choice == 'S':
        print("\nEnter file selection:")
        print("Examples:")
        print("  Individual: 1,3,5")
        print("  Range: 3-8")
        print("  Mixed: 1,3-6,8,10-12")
        
        selection = input("Your selection: ").strip()
        selected_files = parse_file_selection(selection, types_files)
        
        if not selected_files:
            print("❌ Invalid selection format")
            return
            
    else:
        try:
            num = int(choice)
            if 1 <= num <= len(types_files):
                selected_files = [types_files[num-1]]
            else:
                print("❌ Invalid file number")
                return
        except ValueError:
            print("❌ Invalid choice")
            return
    
    if not selected_files:
        print("❌ No files selected")
        return
    
    print(f"\n🚀 Will process {len(selected_files)} files:")
    for file in selected_files:
        print(f"   • {file.name}")
    
    if input("\nContinue? (y/n): ").lower() != 'y':
        return
    
    # Continue with rest of processing...
    all_reports = []
    successful_files = []
    failed_files = []
    
    for i, file in enumerate(selected_files, 1):
        print(f"\n{'='*60}")
        print(f"📁 PROCESSING FILE {i}/{len(selected_files)}: {file.name}")
        print(f"{'='*60}")
        
        try:
            balancer = OptimizedOllamaGearBalancer(str(file))
            
            # Process gear items
            changes = balancer.process_gear_with_progress()
            
            if changes:
                # Generate report
                report = balancer.generate_enhanced_report(changes)
                
                # Save individual report
                timestamp = int(time.time())
                report_path = f'{file.stem}_gear_report_{timestamp}.txt'
                with open(report_path, 'w', encoding='utf-8') as f:
                    f.write(report)
                
                # Save balanced XML
                output_file = balancer.save_balanced_xml()
                
                all_reports.append({
                    'file': file.name,
                    'report': report,
                    'report_path': report_path,
                    'output_file': output_file,
                    'changes_count': len(changes)
                })
                
                successful_files.append(file.name)
                print(f"✅ {file.name} completed - {len(changes)} items processed")
            else:
                failed_files.append(file.name)
                print(f"❌ {file.name} failed - no changes made")
                
        except Exception as e:
            failed_files.append(file.name)
            print(f"❌ Error processing {file.name}: {e}")
            continue
    
    # Generate summary report
    print(f"\n{'='*60}")
    print("📊 BATCH PROCESSING SUMMARY")
    print(f"{'='*60}")
    
    summary_report = generate_batch_summary(all_reports, successful_files, failed_files)
    
    # Save summary
    timestamp = int(time.time())
    summary_path = f'gear_batch_summary_{timestamp}.txt'
    with open(summary_path, 'w', encoding='utf-8') as f:
        f.write(summary_report)
    
    print(summary_report)
    print(f"\n📄 Batch summary saved to: {summary_path}")
    
    if successful_files:
        print(f"\n✅ Successfully processed: {len(successful_files)} files")
        for file in successful_files:
            print(f"   • {file}")
    
    if failed_files:
        print(f"\n❌ Failed files: {len(failed_files)}")
        for file in failed_files:
            print(f"   • {file}")

def parse_file_selection(selection: str, files_list: List[Path]) -> List[Path]:
    """Parse complex file selection patterns like '1,3-6,8,10-12'"""
    selected_indices = set()
    
    try:
        # Split by commas first
        parts = [part.strip() for part in selection.split(',')]
        
        for part in parts:
            if '-' in part:
                # Handle range like "3-8"
                try:
                    start, end = part.split('-', 1)
                    start_num = int(start.strip())
                    end_num = int(end.strip())
                    
                    # Add all numbers in range (inclusive)
                    for num in range(start_num, end_num + 1):
                        if 1 <= num <= len(files_list):
                            selected_indices.add(num - 1)  # Convert to 0-based index
                        
                except ValueError:
                    print(f"❌ Invalid range format: {part}")
                    return []
            else:
                # Handle single number
                try:
                    num = int(part.strip())
                    if 1 <= num <= len(files_list):
                        selected_indices.add(num - 1)  # Convert to 0-based index
                    else:
                        print(f"❌ File number {num} out of range (1-{len(files_list)})")
                        return []
                except ValueError:
                    print(f"❌ Invalid number: {part}")
                    return []
        
        # Convert indices back to file list
        selected_files = [files_list[i] for i in sorted(selected_indices)]
        
        if selected_files:
            print(f"\n📋 Selected {len(selected_files)} files:")
            for i, file in enumerate(selected_files, 1):
                original_index = files_list.index(file) + 1
                print(f"   {i:2d}. {file.name} (#{original_index})")
        
        return selected_files
        
    except Exception as e:
        print(f"❌ Error parsing selection: {e}")
        return []

def generate_batch_summary(reports: List[Dict], successful: List[str], failed: List[str]) -> str:
    """Generate a comprehensive batch processing summary"""
    summary = "🎒 DAYZ GEAR BALANCER - BATCH SUMMARY\n"
    summary += "=" * 60 + "\n\n"
    
    summary += f"📊 OVERVIEW:\n"
    summary += f"   Total Files Processed: {len(successful)}\n"
    summary += f"   Failed Files: {len(failed)}\n"
    summary += f"   Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n\n"
    
    if reports:
        total_items = sum(r['changes_count'] for r in reports)
        summary += f"   Total Gear Items Processed: {total_items}\n"
        summary += f"   Average Items per File: {total_items // len(reports) if reports else 0}\n\n"
    
    # Per-file summary
    if reports:
        summary += "📁 FILE PROCESSING RESULTS:\n"
        summary += "-" * 60 + "\n"
        
        for report in reports:
            summary += f"\n📄 {report['file']}:\n"
            summary += f"   Items Processed: {report['changes_count']}\n"
            summary += f"   Report: {report['report_path']}\n"
            summary += f"   Output: {report['output_file'].name}\n"
            
            # Extract quick stats from report
            lines = report['report'].split('\n')
            for line in lines:
                if 'TIER DISTRIBUTION:' in line:
                    # Find and include tier distribution
                    tier_lines = []
                    idx = lines.index(line) + 1
                    while idx < len(lines) and lines[idx].strip() and not lines[idx].startswith('📂'):
                        if 'Tier' in lines[idx]:
                            tier_lines.append(lines[idx])
                        idx += 1
                    
                    if tier_lines:
                        summary += "   Tier Distribution:\n"
                        for tier_line in tier_lines[:5]:  # Show first 5 tiers
                            summary += f"     {tier_line.strip()}\n"
                    break
    
    if failed:
        summary += f"\n❌ FAILED FILES:\n"
        for file in failed:
            summary += f"   • {file}\n"
    
    summary += f"\n💡 NEXT STEPS:\n"
    summary += f"   1. Review individual reports for detailed analysis\n"
    summary += f"   2. Test balanced XML files on a test server first\n"
    summary += f"   3. Keep backups safe before deploying to production\n"
    summary += f"   4. Monitor server performance after deployment\n"
    
    return summary


def main():
    """Enhanced main execution with single and batch processing options"""
    print("🎒 DayZ Gear & Equipment Balancer with Ollama AI")
    print("=" * 50)
    
    print("Processing modes:")
    print("   1. Single file processing")
    print("   2. Batch process multiple files")
    
    mode = input("\nSelect mode (1 or 2): ").strip()
    
    if mode == '2':
        try:
            # Test Ollama connection first
            print("🔍 Testing Ollama connection...")
            test_response = requests.get("http://localhost:11434/api/tags", timeout=5)
            if test_response.status_code != 200:
                print("❌ Ollama is not responding properly. Make sure it's running:")
                print("   ollama serve")
                return
            print("✅ Ollama connection OK!")
            
            process_multiple_files()
        except KeyboardInterrupt:
            print("\n⚠️ Batch processing interrupted by user")
        except Exception as e:
            print(f"❌ Batch processing error: {e}")
            traceback.print_exc()
        return
    
    # Single file processing (original code)
    try:
        # Test Ollama connection
        print("🔍 Testing Ollama connection...")
        test_response = requests.get("http://localhost:11434/api/tags", timeout=5)
        if test_response.status_code != 200:
            print("❌ Ollama is not responding properly. Make sure it's running:")
            print("   ollama serve")
            return
        
        print("✅ Ollama connection OK!")
        
        # Show available XML files
        xml_files = list(Path('.').glob('*.xml'))
        types_files = [f for f in xml_files if 'types' in f.name.lower() and not any(x in f.name.lower() for x in ['backup', 'balanced', 'weapons'])]
        
        if types_files:
            print(f"\n📁 Available types files:")
            for i, file in enumerate(types_files, 1):
                print(f"   {i}. {file.name}")
        
        # Initialize balancer
        xml_file = input("\n📁 Enter XML file path (or press Enter for 'types.xml'): ").strip()
        if not xml_file:
            xml_file = 'types.xml'
        
        if not Path(xml_file).exists():
            print(f"❌ File not found: {xml_file}")
            return
        
        balancer = OptimizedOllamaGearBalancer(xml_file)
        
        # Get filter options
        filter_mod = input("🔍 Filter by item name (e.g., 'Backpack' or press Enter for all gear): ").strip()
        filter_pattern = filter_mod if filter_mod else None
        
        if filter_pattern:
            preview_items = balancer.collect_gear_items(filter_pattern)
            print(f"📊 Found {len(preview_items)} gear items matching '{filter_pattern}'")
            if len(preview_items) == 0:
                print("❌ No gear items found with that filter. Exiting.")
                return
        else:
            preview_items = balancer.collect_gear_items()
            print(f"📊 Found {len(preview_items)} total gear items to process")
        
        # Show categories that will be processed
        categories = set()
        for item in preview_items[:20]:  # Sample first 20 items
            category_elem = item.find('category')
            if category_elem is not None:
                categories.add(category_elem.get('name', 'unknown'))
        
        print(f"📂 Categories to process: {', '.join(sorted(categories))}")
        
        # Confirm processing
        print(f"\n🚀 Ready to process with model: {balancer.model}")
        print("💡 This will analyze backpacks, clothing, containers, and equipment")
        if input("Continue? (y/n): ").lower() != 'y':
            return
        
        print("\n" + "="*60)
        print("🔄 PROCESSING GEAR & EQUIPMENT...")
        print("="*60)
        
        # Process gear items
        changes = balancer.process_gear_with_progress(filter_pattern=filter_pattern)
        
        # Generate report
        print("\n📊 Generating comprehensive gear report...")
        report = balancer.generate_enhanced_report(changes)
        
        # Save report
        timestamp = int(time.time())
        input_name = Path(xml_file).stem
        report_path = f'{input_name}_gear_report_{timestamp}.txt'
        with open(report_path, 'w', encoding='utf-8') as f:
            f.write(report)
        
        print(f"📄 Report saved to: {report_path}")
        
        # Preview report
        print("\n" + "="*60)
        print("📋 GEAR BALANCE REPORT PREVIEW:")
        print("="*60)
        report_lines = report.split('\n')
        for line in report_lines[:35]:  # Show first 35 lines
            print(line)
        if len(report_lines) > 35:
            print(f"\n... and {len(report_lines) - 35} more lines in full report")
        
        # Show quick statistics
        successful = [c for c in changes if c['applied']]
        tier_counts = {}
        for change in successful:
            tier = change['analysis']['tier']
            tier_counts[tier] = tier_counts.get(tier, 0) + 1
        
        print(f"\n🎯 QUICK STATS:")
        print(f"   ✅ Successfully processed: {len(successful)} items")
        print(f"   🏷️  Tier distribution: {dict(sorted(tier_counts.items()))}")
        
        # Save XML option
        print(f"\n💾 Processing complete! {len(changes)} gear items processed.")
        if input("Save balanced XML file? (y/n): ").lower() == 'y':
            output_file = balancer.save_balanced_xml()
            print(f"✅ Balanced XML saved to: {output_file}")
            print("💡 Remember to backup your original types.xml before using!")
           
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
                print(f"📁 You can manually open: {report_path}")
       
        print("\n🎉 All done! Your DayZ gear is now perfectly balanced!")
        print("💡 Pro tip: Review the report before applying changes to your server")
       
    except KeyboardInterrupt:
        print("\n⚠️ Process interrupted by user")
        print("💡 You can resume processing later if a checkpoint was saved.")
    except FileNotFoundError as e:
        print(f"❌ File not found: {e}")
        print("💡 Make sure your types.xml file exists in the current directory")
    except ET.ParseError as e:
        print(f"❌ XML parsing error: {e}")
        print("💡 Make sure your XML file is valid and well-formed.")
    except requests.exceptions.ConnectionError:
        print("❌ Cannot connect to Ollama. Make sure it's running:")
        print("   ollama serve")
        print("💡 Also ensure you have a model installed:")
        print("   ollama pull llama3.1:8b")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        print("\n🔍 Full error details:")
        traceback.print_exc()
        print("\n💡 Please check your XML file format and Ollama installation")


if __name__ == "__main__":
    main()

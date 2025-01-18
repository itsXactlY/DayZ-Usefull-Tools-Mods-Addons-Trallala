"""
Copyright (c) Dohn Joe 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Description:
This script is designed to parse and process XML data, categorize weapon tiers,
and generate a JSON configuration for a loot system.

Author: Dohn Joe
Email: <redacted>
Date: January 18, 2025
Version: 1.0
"""

import xml.etree.ElementTree as ET
import json
from collections import defaultdict

def load_xml_file(file_path):
    """Load and parse the XML file."""
    try:
        tree = ET.parse(file_path)
        return tree.getroot()
    except ET.ParseError as e:
        print(f"Error parsing XML file: {e}")
        return None
    except FileNotFoundError:
        print(f"File not found: {file_path}")
        return None

def get_all_attachments(item_element):
    """Get all possible attachments for an item."""
    attachments = defaultdict(list)
    presets = []
    magazines = []
    
    for attachment in item_element.findall('attachments'):
        # Handle preset attachments
        preset = attachment.get('preset')
        if preset:
            presets.append(preset)
        
        # Handle specific item attachments
        for item in attachment.findall('item'):
            item_name = item.get('name')
            if item_name:
                if 'mag' in item_name.lower():
                    magazines.append(item_name)
                attachments[categorize_attachment(item_name)].append(item_name)
    
    return dict(attachments), presets, magazines

def categorize_attachment(item_name):
    """Categorize attachment type."""
    name_lower = item_name.lower()
    
    if 'mag_' in name_lower or 'magazine' in name_lower:
        return 'magazine'
    elif 'ammo_' in name_lower or '_rounds' in name_lower:
        return 'ammunition'
    elif any(x in name_lower for x in ['optic', 'scope', 'sight']):
        return 'optic'
    elif any(x in name_lower for x in ['suppressor', 'silencer']):
        return 'suppressor'
    return 'attachment'

def determine_weapon_tier(weapon_name):
    name_lower = weapon_name.lower()
    
    if any(x in name_lower for x in [
        'sniper', 'alligator', '.338', '50cal', 'svd', 'vssk',
        'barrett', 'awm', 'lapua', 'dragunov'
    ]):
        return 'red'
    elif any(x in name_lower for x in [
        'ak74', 'ak101', 'ak103', 'm4a1', 'ar15', 'g36', 'aug',
        'fal', 'scar', 'hk416', 'hk417', 'f2000', 'l85'
    ]):
        return 'purple'
    elif any(x in name_lower for x in [
        'mp5', 'ump45', 'vityaz', 'bizon', 'ak74u',
        'saiga', 'tactical', 'mp7', 'pdw'
    ]):
        return 'blue'
    elif any(x in name_lower for x in [
        'pistol', 'glock', 'cz', 'm1911', 'revolver',
        'shotgun', 'basic', 'hunting'
    ]):
        return 'green'
    return 'yellow'

def generate_loot_entry(weapon_name, attachments, presets, magazines):
    """Generate a loot entry for a weapon."""
    tier = determine_weapon_tier(weapon_name)

    return {
        "dna_Tier": tier,
        "dna_WeaponCategory": "main" if "BO_" in weapon_name else "side",  # Example rule for category
        "dna_TheChosenOne": weapon_name,
        "dna_Magazine": next(iter(attachments.get('magazine', [])), ""),
        "dna_Ammunition": f"Ammo_{weapon_name.replace('BO_', '')}" if weapon_name.startswith('BO_') else "",
        "dna_OpticType": "PSO1Optic" if "scopes" in presets else ("ACOGOptic" if "optics" in presets else ""),
        "dna_Suppressor": "Suppressor" if any("suppressor" in p.lower() for p in presets) else "",
        "dna_UnderBarrel": "",  # Default
        "dna_ButtStock": "",  # Default
        "dna_HandGuard": "",  # Default
        "dna_Wrap": "",  # Default
        "compatible_magazines": magazines,
        "attachment_presets": presets
    }

def main():
    input_file = 'cfgspawnabletypes.xml'
    output_file = 'DNA_Loot_Config.json'
    
    # Initialize tiers
    loot_tiers = {
        "yellow": [],
        "green": [],
        "blue": [],
        "purple": [],
        "red": []
    }
    
    # Load XML
    root = load_xml_file(input_file)
    if root is None:
        return
    
    # Process all weapons
    for item in root.findall('.//type'):
        item_name = item.get('name', '')
        if not item_name or not item_name.startswith('BO_'):
            continue
        
        attachments, presets, magazines = get_all_attachments(item)
        loot_entry = generate_loot_entry(item_name, attachments, presets, magazines)
        loot_tiers[loot_entry["dna_Tier"]].append(loot_entry)
    
    # Wrap all tiers into "m_DNAConfig_Weapons" and save
    output_data = {
        "m_DNAConfig_Weapons": [
            weapon for tier in loot_tiers.values() for weapon in tier
        ]
    }
    
    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(output_data, f, indent=4)
        print(f"Successfully generated loot configuration: {output_file}")
        
        # Print summary
        for tier, items in loot_tiers.items():
            print(f"\nTier {tier}: {len(items)} items")
            for item in items:
                print(f"  - {item['dna_TheChosenOne']}")
                if item['compatible_magazines']:
                    print(f"    Magazines: {', '.join(item['compatible_magazines'])}")
                if item['attachment_presets']:
                    print(f"    Presets: {', '.join(item['attachment_presets'])}")
                
    except IOError as e:
        print(f"Error saving JSON file: {e}")

if __name__ == "__main__":
    main()
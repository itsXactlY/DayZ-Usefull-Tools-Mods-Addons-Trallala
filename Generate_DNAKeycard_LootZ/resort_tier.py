import xml.etree.ElementTree as ET
import random

def categorize_item(item_name):
    # Categorize based on item type and random chance
    weapon_categories = {
        'Tier1': 0.2,  # Damaged, low/no ammo
        'Tier2': 0.3,  # Less damaged, low to medium ammo
        'Tier3': 0.3,  # Close to pristine, medium to high ammo
        'Tier4': 0.2   # Always pristine, full magazines
    }
    
    # Special categories for specific item types
    vehicle_categories = {
        'Tier1': 0.4,  # Ruined
        'Tier2': 0.3,  # Badly damaged
        'Tier3': 0.2,  # Damaged
        'Tier4': 0.1   # Perfect condition
    }
    
    # Other item categories
    other_categories = {
        'Tier1': 0.3,
        'Tier2': 0.3,
        'Tier3': 0.3,
        'Tier4': 0.1
    }
    
    if 'weapon' in item_name.lower() or 'mag' in item_name.lower():
        return random.choices(list(weapon_categories.keys()), 
                               weights=list(weapon_categories.values()))[0]
    elif 'car' in item_name.lower() or 'vehicle' in item_name.lower():
        return random.choices(list(vehicle_categories.keys()), 
                               weights=list(vehicle_categories.values()))[0]
    else:
        return random.choices(list(other_categories.keys()), 
                               weights=list(other_categories.values()))[0]

def update_types_xml(input_path, output_path):
    tree = ET.parse(input_path)
    root = tree.getroot()
    
    for item in root.findall('type'):
        item_name = item.get('name')
        tier = categorize_item(item_name)
        
        # Add usage tag if not exists
        usage_tag = item.find('usage')
        if usage_tag is None:
            usage_tag = ET.SubElement(item, 'usage')
            usage_tag.set('name', 'Generic')
        
        # Add value tag
        value_tag = item.find('value')
        if value_tag is not None:
            value_tag.set('name', tier)
        else:
            value_tag = ET.SubElement(item, 'value')
            value_tag.set('name', tier)
    
    tree.write(output_path, encoding='UTF-8', xml_declaration=True)

# Usage
update_types_xml('types_full.xml', 'updated_types.xml')
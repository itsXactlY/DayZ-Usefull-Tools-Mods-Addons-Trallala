import json
import xml.etree.ElementTree as ET
from pathlib import Path

# Enhanced Category and Tier Configuration
category_mapping = {
    # Weapon Type Keywords
    "Sniper": ("SniperRifle", "Tier4"),
    "DMR": ("MarksmanRifle", "Tier3"),
    "LMG": ("MachineGun", "Tier3"),
    "MG": ("MachineGun", "Tier3"),
    "SMG": ("SubmachineGun", "Tier1"),
    "Shotgun": ("Shotgun", "Tier1"),
    "Pistol": ("Handgun", "Tier1"),
    "Launcher": ("HeavyWeapon", "Tier4"),
    
    # Platform Keywords
    "AR": ("AssaultRifle", "Tier2"),
    "AK": ("AssaultRifle", "Tier2"),
    "HK": ("BattleRifle", "Tier3"),
    "SCAR": ("BattleRifle", "Tier3"),
    "AUG": ("BullpupRifle", "Tier2"),
    "Tavor": ("BullpupRifle", "Tier2"),
    
    # Caliber-Based Fallbacks
    ".50 BMG": ("AntiMaterial", "Tier4"),
    ".408 Cheytac": ("AntiMaterial", "Tier4"),
    ".338 Lapua": ("PrecisionRifle", "Tier4"),
    ".308 Win": ("BattleRifle", "Tier3"),
    "7.62x54mm": ("MachineGun", "Tier3"),
    "5.56x45mm": ("AssaultRifle", "Tier2"),
    "9x39mm": ("Subsonic", "Tier2"),
    "12 Gauge": ("Shotgun", "Tier1"),
    ".45 ACP": ("Handgun", "Tier1"),
    "9x19mm": ("Handgun", "Tier1")
}

# Caliber to Category/Tier Mapping
caliber_to_tier = {
    ".50 BMG": ("AntiMaterial", "Tier4"),
    ".408 Cheytac": ("AntiMaterial", "Tier4"),
    ".338 Lapua Magnum": ("PrecisionRifle", "Tier4"),
    "14.5x114mm": ("AntiMaterial", "Tier4"),
    "12.7x55mm STs-130": ("AntiMaterial", "Tier4"),
    "40MM Grenade": ("HeavyWeapon", "Tier4"),
    ".308 Winchester": ("BattleRifle", "Tier3"),
    ".30-06 Springfield": ("BattleRifle", "Tier3"),
    "7.92x57mm": ("BattleRifle", "Tier3"),
    "7.62x54mm": ("MachineGun", "Tier3"),
    "7.62x39mm": ("AssaultRifle", "Tier2"),
    "5.56x45mm": ("AssaultRifle", "Tier2"),
    "5.45x39mm": ("AssaultRifle", "Tier2"),
    "9x39mm": ("Subsonic", "Tier2"),
    ".300 Blackout": ("AssaultRifle", "Tier2"),
    "9x19mm": ("Handgun", "Tier1"),
    ".45ACP": ("Handgun", "Tier1"),
    ".357 Magnum": ("Handgun", "Tier1"),
    "12 Gauge": ("Shotgun", "Tier1"),
    ".22LR": ("Handgun", "Tier1"),
    "5.7x28mm": ("PDW", "Tier1"),
    "4.6x30mm": ("PDW", "Tier1")
}

# Tier Configuration
tier_config = {
    "Tier4": {
        "usages": ["Military"],
        "values": ["Tier4", "HighValue"],
        "flags": {"map": 1, "cargo": 0, "player": 0}
    },
    "Tier3": {
        "usages": ["Military", "Police"],
        "values": ["Tier3"],
        "flags": {"map": 1, "cargo": 0, "player": 0}
    },
    "Tier2": {
        "usages": ["Police", "Industrial"],
        "values": ["Tier2"],
        "flags": {"map": 1, "cargo": 1, "player": 1}
    },
    "Tier1": {
        "usages": ["Town", "Village"],
        "values": ["Tier1"],
        "flags": {"map": 1, "cargo": 1, "player": 1}
    }
}

def determine_category_and_tier(class_name):
    # First check for weapon type keywords
    for keyword, (category, tier) in category_mapping.items():
        if keyword in class_name:
            return category, tier
    
    # Then check caliber-based mapping
    caliber = weapon_ammo_mapping.get(class_name, "Default")
    return caliber_to_tier.get(caliber, ("Misc", "Tier1"))

# Your provided weapon_ammo_mapping
weapon_ammo_mapping = {
    # ... [Your full weapon_ammo_mapping dictionary here] ...
}

def process_json_file(json_path):
    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        items = data.get("Items", []) if isinstance(data, dict) else []
    except Exception as e:
        print(f"Error processing {json_path}: {str(e)}")
        return

    root = ET.Element("types")
    
    for item in items:
        class_name = item.get("ClassName", "").strip() if isinstance(item, dict) else str(item).strip()
        if not class_name:
            continue

        # Determine category and tier
        category, tier = determine_category_and_tier(class_name)
        cfg = tier_config[tier]

        type_elem = ET.SubElement(root, "type", name=class_name)
        
        # Economic Settings
        ET.SubElement(type_elem, "nominal").text = "10"
        ET.SubElement(type_elem, "lifetime").text = "7200"
        ET.SubElement(type_elem, "restock").text = "1800"
        ET.SubElement(type_elem, "min").text = "5"
        ET.SubElement(type_elem, "quantmin").text = "-1"
        ET.SubElement(type_elem, "quantmax").text = "-1"
        ET.SubElement(type_elem, "cost").text = "100"
        
        # Flags
        flags = ET.SubElement(type_elem, "flags")
        flags.set("count_in_cargo", str(cfg["flags"]["cargo"]))
        flags.set("count_in_hoarder", "0")
        flags.set("count_in_map", str(cfg["flags"]["map"]))
        flags.set("count_in_player", str(cfg["flags"]["player"]))
        flags.set("crafted", "0")
        flags.set("deloot", "0")
        
        # Usages
        for usage in cfg["usages"]:
            ET.SubElement(type_elem, "usage", name=usage)
        
        # Category Tag
        ET.SubElement(type_elem, "tag", name=category)
        
        # Tier Values
        for value in cfg["values"]:
            ET.SubElement(type_elem, "value", name=value)

    xml_path = f"{Path(json_path).stem}_tiered.xml"
    ET.ElementTree(root).write(xml_path, encoding='utf-8', xml_declaration=True)
    print(f"Generated {xml_path} with {len(root)} items")

if __name__ == "__main__":
    for json_file in Path.cwd().glob("*.json"):
        print(f"Processing {json_file.name}...")
        process_json_file(json_file)
    print("Tier sorting complete!")
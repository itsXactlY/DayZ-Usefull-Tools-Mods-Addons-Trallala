import json
import os

# Weapon and ammo mapping for EDO_WEAPONS 
weapon_ammo_mapping = {
    # Assault Rifles
    "BO_AN94": "5.45x39mm", "BO_ACR_Tan": "5.56x45mm", "BO_ACR_Black": "5.56x45mm", "BO_ACR_ODGreen": "5.56x45mm",
    "BO_AR15": ".300 Blackout", "BO_G36C": "5.56x45mm", "BO_G36A1": "5.56x45mm", "BO_FedorovAvtomat": ".30-06 Springfield",
    "BO_SA58": ".308 Winchester", "BO_FNFAL": ".308 Winchester", "BO_HK416": "5.56x45mm", "BO_M110_Tan": ".308 Winchester",
    "BO_SCARL_Black": "5.56x45mm", "BO_SCARL_Tan": "5.56x45mm", "BO_SCARH_Black": ".308 Winchester", "BO_SCARH_Tan": ".308 Winchester",
    "BO_Sig552": "5.56x45mm", "BO_MK18": "5.56x45mm", "BO_AK5C": "5.56x45mm", "BO_F2000": "5.56x45mm", "BO_L85": "5.56x45mm",
    "BO_L85_RIS": "5.56x45mm", "BO_Groza_762": "7.62x39mm", "BO_Groza_9x39": "9x39mm", "BO_M4_15G": "5.56x45mm",
    "BO_M4_15G_FDE": "5.56x45mm", "BO_M4_15G_NoSling": "5.56x45mm", "BO_M4_15G_FDE_NoSling": "5.56x45mm",
    "BO_M4A1_CQB": "5.56x45mm", "BO_FAMAS": "5.56x45mm", "BO_ASVAL": "9x39mm", "BO_AK74": "5.45x39mm", "BO_AKM": "7.62x39mm",
    "BO_AKM_FDE": "7.62x39mm", "BO_ARX160": "7.62x54mm", "BO_G3A3": ".308 Winchester", "BO_HKG28": ".308 Winchester",
    "BO_HoneyBadger": ".300 Blackout", "BO_M16A4": "5.56x45mm", "BO_TAR21": "5.56x45mm", "BO_TKB0146": "5.45x39mm",
    "BO_MDR_Black": "5.56x45mm", "BO_MDR_Tan": "5.56x45mm", "BO_AUGA1": "5.56x45mm", "BO_TavorX95": "5.56x45mm",
    "BO_MCXTacops_Black": "5.56x45mm", "BO_M16A1": "5.56x45mm", "BO_AR10": ".308 Winchester", "BO_AR10_Bakelite": ".308 Winchester",
    "BO_AR10_Shadow": ".308 Winchester", "BO_Beowulf": ".50BW", "BO_AEK971": "5.45x39mm", "BO_CAR15": "5.56x45mm",

    # Rifles
    "BO_M1941": ".30-06 Springfield", "BO_SKS": "7.62x39mm", "BO_SKS_Tactical": "7.62x39mm", "BO_Mk14": ".308 Winchester",
    "BO_M1A_Black": ".308 Winchester", "BO_Winchester1873": ".357 Magnum", "BO_Springfield1903": "", "BO_Delisle": ".45ACP",
    "BO_G43": "7.92x57mm", "BO_Kar98K": "7.92x57mm", "BO_SVT40": "7.62x54mm", "BO_M1Garand": ".30-06 Springfield", 
    "BO_Mini14": "5.56x45mm",

    # SMGs
    "BO_KH9": "9x19mm", "BO_PDWR_556": "5.56x45mm", "BO_PPSh": "7.62x25mm", "BO_Thompson": ".45ACP", 
    "BO_UZI": "9x19mm", "BO_AKS74U": "5.45x39mm", "BO_EVO3A1": "9x19mm", "BO_Mag_EVO_30rnd": "9x19mm", 
    "BO_MP5SDA2": "9x19mm", "BO_MP5SDA2_FDE": "9x19mm", "BO_MP5MPA2": "9x19mm", "BO_MP5MPA2_FDE": "9x19mm", 
    "BO_MP5RailA2": "9x19mm", "BO_MP5RailA2_FDE": "9x19mm", "BO_MKGS": "5.7x28mm", "BO_MP7": "4.6x30mm", 
    "BO_PP1901": "9x19mm", "BO_MP9": "9x19mm", "BO_Vector": ".45ACP", "BO_PP2000": "9x19mm", "BO_UMP45": ".45ACP",

    # LMG's
    "BO_FG42": "7.92x57mm", "BO_FG42M": "7.92x57mm", "BO_RPK16": "5.45x39mm", "BO_Stoner63A": "5.56x45mm", 
    "BO_PKM": "7.62x54mm", "BO_M60": ".308 Winchester", "BO_M240B": ".308 Winchester", "BO_MG42": "7.92x57mm", 
    "BO_M1918": ".30-06 Springfield", "BO_U100": "5.56x45mm", "BO_M249": "5.56x45mm", "BO_M134": ".308 Winchester",
    "BO_PKP_Black": "7.62x54mm", "BO_PKP_FDE": "7.62x54mm",

    # Pistols
    "BO_MagnumSuperSport": ".357 Magnum", "BO_RagingBull": ".357 Magnum", "BO_Tec9": "9x19mm", 
    "BO_Beretta": "9x19mm", "BO_Beretta_Desert": "9x19mm", "BO_DesertEagle": ".50 Action Express", 
    "BO_DesertEagle_BlackSky": ".50 Action Express", "BO_DesertEagle_Gold": ".50 Action Express", 
    "BO_FiveSeven": "5.7x28mm", "BO_Glock18C": "9x19mm", "BO_Glock26": "9x19mm", "BO_Makarov": ".380 Auto", 
    "BO_MK23": ".45ACP", "BO_Hellcat": "9x19mm", "BO_RugerMkIV_Black": "9x19mm", "BO_RugerMkIV_Red": ".22LR", 
    "BO_P30L_Black": "9x19mm",

    # Snipers
    "BO_M200_Grey": ".408 Cheytac", "BO_M200_Black": ".408 Cheytac", "BO_M300": ".408 Cheytac", "BO_MK12": "5.56x45mm",
    "BO_Mosin": "7.62x54mm", "BO_AX50": ".50 BMG", "BO_M82A1": ".50 BMG", "BO_R700": ".308 Winchester", 
    "BO_R700Timberwolf": ".338 Lapua Magnum", "BO_WA2000": ".308 Winchester", "BO_AlligatorSniper": "14.5x114mm",
    "BO_OSV": "12.7x55mm STs-130", "BO_DVL10M1": ".338 Lapua Magnum", "BO_GM6Lynx": ".50 BMG", "BO_L96": ".308 Winchester",
    "BO_M96": ".338 Lapua Magnum", "BO_Ruger": ".338 Lapua Magnum", "BO_SRSA2": ".408 Cheytac", "BO_T5000": ".338 Lapua Magnum",
    "BO_Tac21": ".338 Lapua Magnum", "BO_MRAD": ".338 Lapua Magnum", "BO_SAKOTRG": ".338 Lapua Magnum", "BO_SCAR20S": ".338 Lapua Magnum",
    "BO_SVD": "7.62x54mm", "BO_SuperSniper": "6.5mm Creedmore", "BO_Delta5": "6.5mm Creedmore", "BO_Steam": "7.62x54mm",

    # Shotguns
    "BO_BenelliM4": "12 Gauge", "BO_M870": "12 Gauge", "BO_M1014": "12 Gauge", "BO_Saiga12": "12 Gauge", 
    "BO_SPAS12": "12 Gauge", "BO_WinchesterSXP": "12 Gauge", "BO_M590": "12 Gauge", "BO_MK4_10": "12 Gauge",
    "BO_PancorJackhammer": "12 Gauge", "BO_KSG": "12 Gauge", "BO_HK118": "12 Gauge", "BO_BREACHER": "12 Gauge",
    
    # Grenade Launchers
    "BO_M32_Black": "40MM Grenade"
}

def load_json_file(file_path):
    """Load and parse a JSON file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)
    except (FileNotFoundError, json.JSONDecodeError) as e:
        print(f"Error loading JSON file {file_path}: {e}")
        return None

def load_configs(config_folder):
    """Load all JSON config files from the folder."""
    configs = {}
    for file_name in os.listdir(config_folder):
        if file_name.endswith('.json'):
            file_path = os.path.join(config_folder, file_name)
            config_data = load_json_file(file_path)
            if config_data:
                configs[file_name] = config_data
    return configs


def get_item_data_from_weapon_configs(configs):
    """Extract weapon data from weapon-related configs."""
    item_data = []
    for file_name, config_data in configs.items():
        if file_name.startswith("BO_") and file_name not in ["BO_Ammo.json", "BO_Magazines.json"]:
            for item in config_data.get("Items", []):
                print(f"Processing item: {item['ClassName']}")
                print(f"SpawnAttachments: {item.get('SpawnAttachments', [])}")
                
                item_data.append({
                    "ClassName": item["ClassName"],
                    "Attachments": item.get("SpawnAttachments", []),
                    "Category": config_data.get("DisplayName", "").lower()
                })
    return item_data

def determine_tier_and_category(item):
    """Determine the tier and category of an item based on its name and category."""
    class_name = item["ClassName"].lower()
    category = item["Category"]

    if "sniper" in category or "m200" in class_name:
        return "red", "main"
    elif "rifle" in category or "asr" in class_name:
        return "purple", "main"
    elif "smg" in category or "submachine" in class_name:
        return "blue", "main"
    elif "pistol" in category:
        return "green", "main"
    else:
        return "yellow", "main"

def get_ammo_and_magazines(configs):
    """Parse ammo and magazine configs to map ammo/magazines to weapons."""
    ammo_mapping = {}
    magazine_mapping = {}

    if "BO_Ammo.json" in configs:
        for item in configs["BO_Ammo.json"].get("Items", []):
            weapon_name = item["ClassName"].replace("Ammo_", "").split("_")[0]
            if weapon_name not in ammo_mapping:
                ammo_mapping[weapon_name] = []
            ammo_mapping[weapon_name].append(item["ClassName"])

    if "BO_Magazines.json" in configs:
        for item in configs["BO_Magazines.json"].get("Items", []):
            weapon_name = item["ClassName"].replace("BO_Mag_", "").split("_")[0]
            if weapon_name not in magazine_mapping:
                magazine_mapping[weapon_name] = []
            magazine_mapping[weapon_name].append(item["ClassName"])

    return ammo_mapping, magazine_mapping


def generate_loot_entry(item, tier, category, ammo_mapping, magazine_mapping):
    """Generate a loot entry for a weapon, prioritizing SpawnAttachments and checking for optics."""
    class_name = item["ClassName"]
    weapon_key = class_name.split("_")[1] if "_" in class_name else class_name

    # Extract SpawnAttachments, if present
    spawn_attachments = item.get("Attachments", [])
    
    # More specific optic detection
    spawn_optic = ""
    for attachment in spawn_attachments:
        if attachment.endswith("_Optic"):
            spawn_optic = attachment
            break
    
    # Find specific attachment types
    spawn_mag = next((att for att in spawn_attachments if "Mag_" in att or "_Mag_" in att), "")
    spawn_ammo = next((att for att in spawn_attachments if "Ammo" in att), "")

    # Try to find compatible ammo and magazines using mappings
    compatible_ammo = ammo_mapping.get(weapon_key, [])
    compatible_mags = magazine_mapping.get(weapon_key, [])

    # Fallback: Use weapon_ammo_mapping if no compatible ammo/mags are found
    fallback_ammo = weapon_ammo_mapping.get(class_name, "")
    fallback_magazine = f"Mag_{class_name}"  # Assuming a convention for magazines

    # Resolve ammo and magazine using priority order: SpawnAttachments > Mappings > Fallback
    resolved_ammo = spawn_ammo or (compatible_ammo[0] if compatible_ammo else fallback_ammo)
    resolved_mag = spawn_mag or (compatible_mags[0] if compatible_mags else fallback_magazine)

    # debugging
    print(f"Debug - {class_name} attachments: {spawn_attachments}")
    print(f"Debug - Found optic: {spawn_optic}")

    return {
        "dna_Tier": tier,
        "dna_WeaponCategory": category,
        "dna_TheChosenOne": class_name,
        "dna_Magazine": resolved_mag,
        "dna_Ammunition": resolved_ammo,
        "dna_OpticType": spawn_optic,
        "dna_Suppressor": "",
        "dna_UnderBarrel": "",
        "dna_ButtStock": "",
        "dna_HandGuard": "",
        "dna_Wrap": ""
    }

def get_item_data_from_weapon_configs(configs):
    """Extract weapon data from weapon-related configs."""
    item_data = []
    for file_name, config_data in configs.items():
        if file_name.startswith("BO_") and file_name not in ["BO_Ammo.json", "BO_Magazines.json"]:
            for item in config_data.get("Items", []):
                item_data.append({
                    "ClassName": item["ClassName"],
                    "Attachments": item.get("SpawnAttachments", []),
                    "Category": config_data.get("DisplayName", "").lower()
                })
    return item_data


def main():
    config_folder = "Market"
    output_file = "DNA_Tier_Config.json"

    # Load all configs
    configs = load_configs(config_folder)

    # Get ammo and magazine mappings
    ammo_mapping, magazine_mapping = get_ammo_and_magazines(configs)

    # Get weapon data, including attachments and categories
    item_data = get_item_data_from_weapon_configs(configs)
    loot_entries = []

    # Process each weapon and assign to a tier
    for item in item_data:
        tier, category = determine_tier_and_category(item)
        loot_entry = generate_loot_entry(item, tier, category, ammo_mapping, magazine_mapping)
        loot_entries.append(loot_entry)

    # Wrap all loot entries into "m_DNAConfig_Weapons" and save
    output_data = {
        "m_DNAConfig_Weapons": loot_entries
    }

    try:
        with open(output_file, "w", encoding="utf-8") as f:
            json.dump(output_data, f, indent=4)
        print(f"Successfully generated DNA tier configuration: {output_file}")

        # summary
        print(f"\nGenerated {len(loot_entries)} weapon entries.")
        for entry in loot_entries:
            print(
                f"  - {entry['dna_TheChosenOne']}: Tier {entry['dna_Tier']} "
                f"(Ammo: {entry['dna_Ammunition']}, Mag: {entry['dna_Magazine']}, Optic: {entry['dna_OpticType']})"
            )

    except IOError as e:
        print(f"Error saving JSON file: {e}")


if __name__ == "__main__":
    main()

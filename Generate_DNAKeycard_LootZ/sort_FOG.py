import xml.etree.ElementTree as ET

# Define your rebalancing rules here
default_nominal = 1
default_lifetime = 1800
default_restock = 1200
default_min = 0
default_usages = ["Police", "Military"]  # Add more as needed

# Custom categories based on item name
category_mapping = {
    "Pants": "clothes",
    "Shirt": "clothes",
    "Jacket": "clothes",
    "Shoes": "clothes",
    "Helmet": "clothes",
    "Vest": "clothes",
    "Backpack": "clothes",
    "Gun": "weapons",
    "Rifle": "weapons",
    "Pistol": "weapons",
    "Knife": "weapons",
    "Ammo": "ammunition",
    "Food": "food",
    "Drink": "food",
    # Add more mappings as needed
}

# Determine the category based on the item name
def determine_category(item_name):
    for keyword, category in category_mapping.items():
        if keyword in item_name:
            return category
    return "misc"  # Default category if no match is found

# Rebalancing function for FOG items
def rebalance_fog_item(item):
    # Adjust attributes based on rules
    item.find("nominal").text = str(default_nominal)
    item.find("lifetime").text = str(default_lifetime)
    item.find("restock").text = str(default_restock)
    item.find("min").text = str(default_min)
    
    # Update usages
    usages = item.findall("usage")
    for usage in usages:
        item.remove(usage)  # Clear existing usages
    for usage_name in default_usages:
        usage_element = ET.SubElement(item, "usage")
        usage_element.set("name", usage_name)
    
    # Update or set the category
    category_element = item.find("category")
    if category_element is None:
        category_element = ET.SubElement(item, "category")
    category_name = determine_category(item.get("name", ""))
    category_element.set("name", category_name)

# Function to process types.xml
def process_types_file(input_file, output_file):
    # Parse the XML file
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Process all FOG_ items
    for item in root.findall("type"):
        name = item.get("name", "")
        if name.startswith("FOG_"):  # Only process items starting with "FOG_"
            rebalance_fog_item(item)
    
    # Write the updated XML back to the file
    tree.write(output_file, encoding="utf-8", xml_declaration=True)

# Specify input and output files
input_file = "types.xml"  # Replace with your existing types.xml
output_file = "types_rebalanced.xml"  # This will be the rebalanced file

# Process the file
process_types_file(input_file, output_file)
import json
import os

def generate_types_xml(json_files, output_file="types_magazines.xml"):
    """
    Generate a types.xml file from given market JSON files.
    
    :param json_files: List of JSON file paths to process.
    :param output_file: Path to the output types.xml file.
    """
    types_entries = []

    for json_file in json_files:
        with open(json_file, 'r') as f:
            data = json.load(f)
            items = data.get("Items", [])
            
            for item in items:
                class_name = item.get("ClassName")
                if class_name:
                    entry = f"""
    <type name="{class_name}">
        <nominal>50</nominal>
        <lifetime>3600</lifetime>
        <restock>3600</restock>
        <cost>50</cost>
        <flags count_in_cargo="1" count_in_hoarder="1" count_in_map="1" count_in_player="1" crafted="0" deloot="0" />
        <category name="default" />
    </type>
                    """.strip()
                    types_entries.append(entry)

    # Combine entries into the final XML
    xml_content = f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<types>
{os.linesep.join(types_entries)}
</types>
    """

    # Write the XML content to the output file
    with open(output_file, 'w') as f:
        f.write(xml_content)

    print(f"types.xml has been generated successfully at {output_file}")


# Specify the input JSON files and run the script
json_files = ["BO_Magazines.json"]  # Replace with the actual paths to your JSON files
generate_types_xml(json_files)

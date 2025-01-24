import xml.etree.ElementTree as ET
import random

def load_xml(filename):
    tree = ET.parse(filename)
    return tree.getroot()

def find_matching_attachments(types, gun_type):
    optics = []
    magazines = []
    
    # Find random optic
    for optic in types['optics'].findall('type'):
        if optic.get('name').startswith('BO_'):
            optics.append(optic.get('name'))
    
    # Find matching magazine
    for mag in types['magazines'].findall('type'):
        mag_name = mag.get('name')
        if mag_name.startswith('BO_Mag_') and gun_type.replace('BO_', '').lower() in mag_name.lower():
            magazines.append(mag_name)
    
    return {
        'optics': random.choice(optics) if optics else None,
        'magazines': magazines
    }

def update_types_xml(types_path, optics_path, magazines_path):
    types = ET.parse(types_path)
    types_root = types.getroot()
    
    types_dict = {
        'optics': ET.parse(optics_path).getroot(),
        'magazines': ET.parse(magazines_path).getroot()
    }
    
    for gun_type in types_root.findall('type'):
        gun_name = gun_type.get('name')
        
        # Skip if already has attachments
        if gun_type.find('attachments') is not None:
            continue
        
        attachments = find_matching_attachments(types_dict, gun_name)
        
        if attachments['optics'] or attachments['magazines']:
            attachments_elem = ET.SubElement(gun_type, 'attachments')
            
            if attachments['optics']:
                optic_elem = ET.SubElement(attachments_elem, 'attachment')
                optic_elem.set('name', attachments['optics'])
            
            for mag in attachments['magazines']:
                mag_elem = ET.SubElement(attachments_elem, 'attachment')
                mag_elem.set('name', mag)
    
    types.write('updated_types.xml', encoding='UTF-8', xml_declaration=True)

# Usage
update_types_xml('types.xml', 'types_optics.xml', 'types_magazines.xml')
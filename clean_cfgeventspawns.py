import xml.etree.ElementTree as ET
import math

def calculate_distance(pos1, pos2):
    x1 = float(pos1.get('x'))
    z1 = float(pos1.get('z'))
    x2 = float(pos2.get('x'))
    z2 = float(pos2.get('z'))
    
    return math.sqrt((x2 - x1)**2 + (z2 - z1)**2)

def remove_nearby_positions(xml_file):
    vehicle_events = {
        "VehicleBoat": 500,
        "VehicleCivilianSedan": 1000,
        "VehicleSedan02": 1000,
        "VehicleHatchback02": 1000,
        "VehicleOffroad02": 1000,
        "VehicleTruck01": 1000
    }

    try:
        tree = ET.parse(xml_file)
        root = tree.getroot()
    except Exception as e:
        print(f"Error parsing XML file: {e}")
        return

    new_root = ET.Element('eventposdef')
    
    for event in root.findall('event'):
        event_name = event.get('name')
        
        if event_name not in vehicle_events:
            new_root.append(event)
            continue

        distance_threshold = vehicle_events[event_name]
        
        positions = event.findall('pos')
        positions_to_remove = set()
        
        # Compare positions
        for i, pos1 in enumerate(positions):
            for j, pos2 in enumerate(positions[i+1:], i+1):
                if calculate_distance(pos1, pos2) < distance_threshold:
                    positions_to_remove.add(j)
        
        new_event = ET.SubElement(new_root, 'event')
        new_event.set('name', event_name)
        
        for i, pos in enumerate(positions):
            if i not in positions_to_remove:
                new_pos = ET.SubElement(new_event, 'pos')
                for attr in ['x', 'z', 'a']:
                    if pos.get(attr):
                        new_pos.set(attr, pos.get(attr))
        
        print(f"Event {event_name} (radius: {distance_threshold}m):")
        print(f"  Original positions: {len(positions)}")
        print(f"  Positions after filtering: {len(positions) - len(positions_to_remove)}")
        print(f"  Removed positions: {len(positions_to_remove)}")
        print()

    output_file = 'filtered_' + xml_file
    try:
        new_tree = ET.ElementTree(new_root)
        new_tree.write(output_file, encoding='utf-8', xml_declaration=True)
        print(f"Output saved to: {output_file}")
    except Exception as e:
        print(f"Error writing to file: {e}")
        return

if __name__ == "__main__":
    xml_file = "cfgeventspawns.xml"
    remove_nearby_positions(xml_file)

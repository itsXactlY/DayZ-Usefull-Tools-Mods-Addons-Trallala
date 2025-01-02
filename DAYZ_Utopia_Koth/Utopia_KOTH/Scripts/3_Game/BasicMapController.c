// Handling map markers for the KOTH mod
#ifdef BASICMAP
modded class BasicMapController
{
    // Remove marker by vector position for KOTH mod
    bool KothRemoveMarkerByVector(vector pos, float distance = 10, bool overrideSettings = false)
    {      
        for (int j = 0; j < Markers.Count(); j++)
        {
            for (int k = 0; k < Markers.GetElement(j).Count(); k++)
            {
                if (vector.Distance(Markers.GetElement(j).Get(k).GetPosition(), pos) < distance && (Markers.GetElement(j).Get(k).Editable() || overrideSettings))
                {
                    OnMarkerDelete(Markers.GetElement(j).Get(k));
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(Markers.GetElement(j).RemoveOrdered, k);
                    return true;
                }
            }
        }
        return false;
    }

    // Remove marker by marker object for KOTH mod
    bool KothRemoveMarker(BasicMapMarker marker)
    {
        for (int j = 0; j < Markers.Count(); j++)
        {
            for (int k = 0; k < Markers.GetElement(j).Count(); k++)
            {
                if (marker == Markers.GetElement(j).Get(k))
                {
                    OnMarkerDelete(marker);
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(Markers.GetElement(j).RemoveOrdered, k);
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateServerMarker, 500, false);
                    return true;
                }
            }
        }
        return false;
    }

    // Update all server markers for the KOTH mod
    void UpdateServerMarker()
    {
        array<autoptr BasicMapMarker> ServerMarkers = new array<autoptr BasicMapMarker>;
        array<autoptr BasicMapCircleMarker> ServerCircleMarkers = new array<autoptr BasicMapCircleMarker>;

        if (Markers.Get(SERVER_KEY))
        {
            BasicMapCircleMarker cMarker;
            for (int j = 0; j < Markers.Get(SERVER_KEY).Count(); j++)
            {
                if (Markers.Get(SERVER_KEY).Get(j).IsInherited(BasicMapCircleMarker) && Class.CastTo(cMarker, Markers.Get(SERVER_KEY).Get(j)) && cMarker.GetRadius() > 0)
                {
                    ServerCircleMarkers.Insert(cMarker);
                }
                else
                {
                    ServerMarkers.Insert(Markers.Get(SERVER_KEY).Get(j));
                }
            }
        }

        GetRPCManager().SendRPC("BasicMap", "RPCSyncServerData", new Param3<array<autoptr BasicMapMarker>, array<autoptr BasicMapCircleMarker>, ref BasicMapConfig>(ServerMarkers, ServerCircleMarkers, GetBasicMapConfig()), true, null);
    }

    // Create and add a KOTH event marker to the map
    BasicMapMarker CreateKOTHServerMarker(string name, vector position)
    {
        array<autoptr BasicMapMarker> ServerMarkers = new array<autoptr BasicMapMarker>;
        array<autoptr BasicMapCircleMarker> ServerCircleMarkers = new array<autoptr BasicMapCircleMarker>;
        string markerIcon = "Utopia_KOTH\\datasets\\satcom.paa";  // Set your KOTH marker icon path
        bool isEditable = false;
        BasicMapMarker serverMarker = new BasicMapMarker(name, position, markerIcon, {255, 0, 0}, 255, isEditable);
        
        AddMarker(BasicMapController.SERVER_KEY, serverMarker);

        if (Markers.Get(SERVER_KEY))
        {
            BasicMapCircleMarker cMarker;
            for (int j = 0; j < Markers.Get(SERVER_KEY).Count(); j++)
            {
                if (Markers.Get(SERVER_KEY).Get(j).IsInherited(BasicMapCircleMarker) && Class.CastTo(cMarker, Markers.Get(SERVER_KEY).Get(j)) && cMarker.GetRadius() > 0)
                {
                    ServerCircleMarkers.Insert(cMarker);
                }
                else
                {
                    ServerMarkers.Insert(Markers.Get(SERVER_KEY).Get(j));
                }
            }
        }

        GetRPCManager().SendRPC("BasicMap", "RPCSyncServerData", new Param3<array<autoptr BasicMapMarker>, array<autoptr BasicMapCircleMarker>, ref BasicMapConfig>(ServerMarkers, ServerCircleMarkers, GetBasicMapConfig()), true, null);
        
        return serverMarker;
    }
}
#endif
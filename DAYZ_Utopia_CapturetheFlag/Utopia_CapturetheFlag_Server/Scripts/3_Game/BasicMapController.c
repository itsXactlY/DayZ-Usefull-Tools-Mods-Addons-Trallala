#ifdef BASICMAP
modded class BasicMapController
{

    private static ref BasicMapController s_Instance;

    void BasicMapController() {
        // Constructor code here (if needed)
    }

    static BasicMapController GetInstance() {
        if (!s_Instance) {
            s_Instance = new BasicMapController();
        }
        return s_Instance;
    }

    
    // Remove marker by vector position for CTF mod
    bool CTFRemoveMarkerByVector(vector pos, float distance = 10, bool overrideSettings = false)
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

    // Remove marker by marker object for CTF mod
    bool CTFRemoveMarker(BasicMapMarker marker)
    {
        for (int j = 0; j < Markers.Count(); j++)
        {
            for (int k = 0; k < Markers.GetElement(j).Count(); k++)
            {
                if (marker == Markers.GetElement(j).Get(k))
                {
                    OnMarkerDelete(marker);
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(Markers.GetElement(j).RemoveOrdered, k);
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UpdateCTFServerMarker, 500, false);
                    return true;
                }
            }
        }
        return false;
    }

    // Update all server markers for the CTF mod
    void UpdateCTFServerMarker()
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

    // Update the position of an existing marker
    void UpdateMarkerPosition(BasicMapMarker marker, vector newPosition) 
    {
        if (marker) {
            marker.SetPosition(newPosition);
            //UtopiaLogger.Log("Updated BasicMap marker position to: " + newPosition.ToString() + " for marker: " + marker.GetName(), LOG_TYPE);
            UpdateCTFServerMarker(); // Sync the marker update with the server
        }
    }


    // Create and add a CTF event marker to the map
    BasicMapMarker CreateCTFServerMarker(string name, vector position)
    {
        array<autoptr BasicMapMarker> ServerMarkers = new array<autoptr BasicMapMarker>;
        array<autoptr BasicMapCircleMarker> ServerCircleMarkers = new array<autoptr BasicMapCircleMarker>;
        string markerIcon = "Utopia_CapturetheFlag\\Datasets\\flag_icon.paa";  // Set your CTF marker icon path
        bool isEditable = false;
        BasicMapMarker serverMarker = new BasicMapMarker(name, position, markerIcon, {0, 255, 0}, 255, isEditable);
        
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

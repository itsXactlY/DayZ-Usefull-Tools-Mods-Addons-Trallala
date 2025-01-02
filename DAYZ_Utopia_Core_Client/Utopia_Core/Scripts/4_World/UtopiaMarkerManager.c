class UtopiaMarkerManager
{
    // Constants for marker icons and log types
    static const string KOTH_MARKER_ICON = "Utopia_KOTH\\datasets\\satcom.paa"; //skip
    static const string AIRDROP_MARKER_ICON = "Utopia_Airdrop\\Datasets\\Images\\airdrop.paa"; //skip
    static const string CTF_MARKER_ICON = "Utopia_CapturetheFlag\\Datasets\\flag_icon.paa"; //skip
    static const string LOG_TYPE = "MarkerManager"; //skip

    // Unique ID counter for all markers
    private static int s_currentMarkerId = 0; //skip

    // Containers for managing markers based on type (e.g., LB markers)
    #ifdef LBmaster_Groups
    private static ref map<int, LBServerMarker> mapMarkerLB = new map<int, LBServerMarker>(); //skip
    #endif

    // =========================
    // Marker Creation Functions
    // =========================

    // Create a new KOTH marker on the map using LB marker system
    static int CreateKothMarker(string name, vector position)
    {
        UtopiaLogger.Log("Creating KOTH marker: " + name + " at position " + position, LOG_TYPE);

        #ifdef LBmaster_Groups
        int lbMarkerId = AddLBMarker(name, position, KOTH_MARKER_ICON, {255, 0, 0});
        if (lbMarkerId != -1)
        {
            UtopiaLogger.Log("KOTH LB marker created with ID: " + lbMarkerId, LOG_TYPE);
            return lbMarkerId;  // Return the marker ID
        }
        else
        {
            UtopiaLogger.Log("Failed to create KOTH LB marker", LOG_TYPE);
            return -1;  // Indicate failure
        }
        #endif

        return -1;  // In case LBmaster_Groups is not defined
    }


    // Create a new Airdrop marker on the map using LB marker system
    static void CreateAirdropMarker(string name, vector position) //skip
    {
        UtopiaLogger.Log("Creating Airdrop marker: " + name + " at position " + position, LOG_TYPE); //skip

        #ifdef LBmaster_Groups
        int lbMarkerId = AddLBMarker(name, position, AIRDROP_MARKER_ICON, {255, 188, 0}); //skip
        if (lbMarkerId != -1) //skip
        {
            UtopiaLogger.Log("Airdrop LB marker created with ID: " + lbMarkerId, LOG_TYPE); //skip
        }
        #endif
    }

    // Create a new CTF marker on the map using LB marker system
    static int CreateCTFMarker(string name, vector position) //skip
    {
        UtopiaLogger.Log("Creating CTF marker: " + name + " at position " + position, LOG_TYPE); //skip

        #ifdef LBmaster_Groups
        int lbMarkerId = AddLBMarker(name, position, CTF_MARKER_ICON, {0, 255, 0}); //skip
        if (lbMarkerId != -1) //skip
        {
            UtopiaLogger.Log("CTF LB marker created with ID: " + lbMarkerId, LOG_TYPE); //skip
            return lbMarkerId; //skip
        }
        #endif

        return -1; //skip
    }

    // =========================
    // Marker Creation Helper Functions
    // =========================

    #ifdef LBmaster_Groups
    // General function to create an LB marker on the map
    static int AddLBMarker(string name, vector position, string iconPath, array<int> color) //skip
    {
        LBServerMarker marker = LBStaticMarkerManager.Get.AddTempServerMarker(name, position, iconPath, ARGB(255, color[0], color[1], color[2]), false, true, true, true); //skip

        if (!marker) //skip
        {
            UtopiaLogger.Log("Failed to create LB marker for: " + name, LOG_TYPE); //skip
            return -1;  // Indicate failure //skip
        }

        ++s_currentMarkerId; //skip
        mapMarkerLB.Set(s_currentMarkerId, marker); //skip
        UtopiaLogger.Log("Added LB marker with ID: " + s_currentMarkerId, LOG_TYPE); //skip
        return s_currentMarkerId; //skip
    }

    static void UpdateCTFMarkerPosition(int id, vector position)
    {
        #ifdef LBmaster_Groups
        LBServerMarker marker = mapMarkerLB.Get(id);
        if (marker) 
        {
            marker.SetPosition(position);
            UtopiaLogger.Log("[UtopiaMarkerManager] Updated CTF LB marker ID: " + id + " to new position: " + position.ToString(), LOG_TYPE);
        } 
        else 
        {
            UtopiaLogger.Log("[UtopiaMarkerManager] Failed to update CTF LB marker. ID: " + id + " not found.", LOG_TYPE);
        }
        #endif
    }

    // Update an existing LB marker's position
    static void UpdateLBMarkerPosition(int markerId, vector newPosition) //skip
    {
        if (!mapMarkerLB.Contains(markerId)) //skip
        {
            UtopiaLogger.Log("Cannot update marker position. Marker ID not found: " + markerId, LOG_TYPE); //skip
            return; //skip
        }

        LBServerMarker marker = mapMarkerLB.Get(markerId); //skip
        marker.SetPosition(newPosition); //skip
        UtopiaLogger.Log("Updated LB marker position for ID: " + markerId + " to new position: " + newPosition.ToString(), LOG_TYPE); //skip
    }

    // Remove an LB marker by ID
    static void RemoveLBMarker(int id) //skip
    {
        LBServerMarker marker = mapMarkerLB.Get(id); //skip
        UtopiaLogger.Log("Removing LB marker with ID: " + id, LOG_TYPE); //skip

        if (marker) //skip
        {
            LBStaticMarkerManager.Get.RemoveServerMarker(marker); //skip
            mapMarkerLB.Remove(id); //skip
            UtopiaLogger.Log("LB marker removed: ID " + id, LOG_TYPE); //skip
        }
    }
    #endif

    // Remove all related markers when the event ends (optional cleanup)
    static void CleanupMarkers() //skip
    {
        UtopiaLogger.Log("Cleaning up all event markers.", LOG_TYPE); //skip
        #ifdef LBmaster_Groups
        foreach (int id, LBServerMarker marker : mapMarkerLB) //skip
        {
            LBStaticMarkerManager.Get.RemoveServerMarker(marker); //skip
        }
        mapMarkerLB.Clear(); //skip
        UtopiaLogger.Log("All LB markers cleared.", LOG_TYPE); //skip
        #endif
    }
}

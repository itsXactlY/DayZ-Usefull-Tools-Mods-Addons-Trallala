class KothDCfg
{	
   string NotificationHeading;
   string NotificationEnter;
   string NotificationExit;
   string NotificationStartEvent;
   string NotificationEndEvent;
   string NotificationCancelEvent;
   int PlayerLimitToStart;
   int MaxCountOfEvents;
   float MinTimeBetweenEvents;
   float MaxTimeBetweenEvents;
   ref array<ref KothDList> KothDCfgList = new array<ref KothDList>; 
};


class KothDList
{
    bool IsTriggerActive;
    string Cords;
    string KothName;
    float DeleteDelayTime;	
    int MinPlayersToCapture;
    float TriggerRadius;
    float TriggerTimerBeforDeleteWithoutPlayers;
    float BonusLostTimeForEachPlayerOverThreshold;
    float CaptureTime;	
    vector AirdropFlarePosition;

    // Enemy Waves
    ref array<ref WaveData> EnemyWaves; 
    ref array<int> CaptureProgressTriggers; 

    // Loot Configurations
    bool UseAirdropFlare; 
    string LootContainerClass; 
    ref array<ref LootItem> LootItems; 
    vector LootContainerPosition;
    vector LootContainerOrientation;

    // Building Set Configuration
    string BuildingSetFile;

    void KothDList()
    {
        LootItems = new array<ref LootItem>();
        EnemyWaves = new array<ref WaveData>;
        CaptureProgressTriggers = new array<int>;
    }
}


class WaveData
{
    ref array<string> EntitiesToSpawn;

    void WaveData()
    {
        EntitiesToSpawn = new array<string>;
    }
};


class KothConfig
{
    const static string cfgPathWay = "$profile:Utopia_Koth/Koth.json";
	const static string dirPathName = "$profile:\\Utopia_Koth";
	
    ref KothDCfg cfg;	
	ref array<Man> players = new array<Man>; 
	ref array<KothPoint> kothlist = new array<KothPoint>;	
	ref array<vector> m_KothPos = new array<vector>;
	ref array<float> m_Remainingtime = new array<float>;
	ref array<int> usedKothLocations = new array<int>; // New array to track used locations
	vector lastKothPosition;
	ref KothBuildingSetsLoader m_BuildingLoader;
	ref Utopia_KothUI m_KothUI;
	
    void KothConfig()
    {
		cfg = new KothDCfg();

		Print("[KothConfig] config found, loading...");

		CleanupExistingKOTHObjects();
        LoadKothCFG();				
    }


    void LoadKothCFG()
    {		
		if (FileExist(cfgPathWay))
		{
			JsonFileLoader<KothDCfg>.JsonLoadFile(cfgPathWay, cfg);
		}
        else
		{
			CreateKothConfig();
		}	
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StartEvent, 60000, false);			
    }

    private void SaveKothCFG()
    {
        JsonFileLoader<KothDCfg>.JsonSaveFile( cfgPathWay, cfg );
    }

	private void GenerateDefaultBuildingSetFile()
	{
		string dirPath = "$profile:Utopia_Koth/Objects/";
		string buildingSetFile = dirPath + "default_buildings.json";
		
		// Ensure the directory exists before attempting to create the file
		if (!FileExist(dirPath))
		{
			Print("[KothConfig] '" + dirPath + "' does not exist, creating...");
			MakeDirectory(dirPath);
		}

		if (!FileExist(buildingSetFile))
		{
			Print("[KothConfig] Creating default building set JSON file: " + buildingSetFile);
			ref array<ref KothBuilding> defaultBuildings = new array<ref KothBuilding>;

			// Create default buildings
			KothBuilding building1 = new KothBuilding("Land_Mil_Barracks1", "5000 0 5000".ToVector(), "0 0 0".ToVector());//skip
			KothBuilding building2 = new KothBuilding("Land_Mil_Barracks1", "5005 0 5000".ToVector(), "0 0 0".ToVector());//skip
			KothBuilding building3 = new KothBuilding("Land_Mil_Barracks1", "5010 0 5000".ToVector(), "0 0 0".ToVector());//skip

			// Add to the array
			defaultBuildings.Insert(building1);
			defaultBuildings.Insert(building2);
			defaultBuildings.Insert(building3);

			// Save the array to a JSON file
			JsonFileLoader<array<ref KothBuilding>>.JsonSaveFile(buildingSetFile, defaultBuildings);

			Print("[KothConfig] Default building set JSON file created.");
		}
		else
		{
			Print("[KothConfig] Building set file already exists: " + buildingSetFile);
		}
	}


	
    private void CreateKothConfig()
	{	
		if (!FileExist(dirPathName))
		{
			Print("[KothConfig] '" + dirPathName + "' does not exist, creating...");
			MakeDirectory(dirPathName);
		}
		
		cfg.NotificationHeading = "Attention!";
		cfg.NotificationEnter = "You have entered the KOTH";
		cfg.NotificationExit = "You are leaving the KOTH";
		cfg.NotificationStartEvent = "The battle has begun at %1";
		cfg.NotificationEndEvent = "The battle has ended at %1";
		cfg.NotificationCancelEvent = "The battle at %1 has been canceled";
		cfg.PlayerLimitToStart = 1;
		cfg.MaxCountOfEvents = 1;
		cfg.MinTimeBetweenEvents = 60;
		cfg.MaxTimeBetweenEvents = 120;
		
		KothDList kth1 = new KothDList;
		kth1.IsTriggerActive = true;
		kth1.KothName = "KOTH Point 1";
		kth1.Cords = "1000 0 1000|0 0 0"; //skip
		kth1.DeleteDelayTime = 600; // Default delay time
		kth1.MinPlayersToCapture = 1;
		kth1.TriggerRadius = 20;
		kth1.TriggerTimerBeforDeleteWithoutPlayers = 600;
		kth1.BonusLostTimeForEachPlayerOverThreshold = 0.3;
		kth1.CaptureTime = 10;
		kth1.UseAirdropFlare = true;
		kth1.LootContainerClass = "SeaChest";
		kth1.AirdropFlarePosition = "5000 0 5000".ToVector();  // Example coordinates for flare position
		kth1.LootContainerPosition = "0 0 0";  //skip
		kth1.LootContainerOrientation = "0 0 0";  //skip
		LootItem kth1_item1 = new LootItem("AKM", -1, 0.5);  // 50% chance to spawn AK47
		kth1_item1.Attachments.Insert(new LootItem("AK_Suppressor", -1, 0.5));  // 50% chance
		kth1_item1.Attachments.Insert(new LootItem("PSO1Optic", -1, 0.7));      // 70% chance
		kth1.LootItems.Insert(kth1_item1);
		LootItem kth1_item2 = new LootItem("M4A1", -1, 0.3);  // 30% chance to spawn M4A1
		kth1_item2.Attachments.Insert(new LootItem("M4_Suppressor", -1, 0.4));  // 40% chance
		kth1.LootItems.Insert(kth1_item2);
		kth1.BuildingSetFile = "default_buildings.json";  // Use unique building set file for Point 1
		// Enemy Waves Configuration for kth1
		// Define capture progress triggers (e.g., 25%, 50%, 75%)
		kth1.CaptureProgressTriggers.Insert(25);
		kth1.CaptureProgressTriggers.Insert(50);
		kth1.CaptureProgressTriggers.Insert(75);

		// Define enemy waves corresponding to the triggers

		// Wave 1
		WaveData wave1_kth1 = new WaveData();
		wave1_kth1.EntitiesToSpawn.Insert("ZmbM_CitizenASkinny_Brown");
		wave1_kth1.EntitiesToSpawn.Insert("ZmbM_HermitSkinny_Beige");
		kth1.EnemyWaves.Insert(wave1_kth1);

		// Wave 2
		WaveData wave2_kth1 = new WaveData();
		wave2_kth1.EntitiesToSpawn.Insert("ZmbM_CitizenASkinny_Brown");
		wave2_kth1.EntitiesToSpawn.Insert("ZmbM_HermitSkinny_Beige");
		kth1.EnemyWaves.Insert(wave2_kth1);

		// Wave 3
		WaveData wave3_kth1 = new WaveData();
		wave3_kth1.EntitiesToSpawn.Insert("ZmbM_PolicemanFat");
		wave3_kth1.EntitiesToSpawn.Insert("ZmbM_PolicemanFat");
		kth1.EnemyWaves.Insert(wave3_kth1);
		
		cfg.KothDCfgList.Insert(kth1);

		KothDList kth2 = new KothDList;
		kth2.IsTriggerActive = true;
		kth2.KothName = "KOTH Point 2";
		kth2.Cords = "2000 0 2000|0 0 0"; //skip
		kth2.DeleteDelayTime = 600;
		kth2.MinPlayersToCapture = 1;
		kth2.TriggerRadius = 20;
		kth2.TriggerTimerBeforDeleteWithoutPlayers = 600;
		kth2.BonusLostTimeForEachPlayerOverThreshold = 0.3;
		kth2.CaptureTime = 10;
		kth2.UseAirdropFlare = true;
		kth2.LootContainerClass = "SeaChest";
		kth2.AirdropFlarePosition = "5000 0 5000".ToVector();  // Example coordinates for flare position
		kth2.LootContainerPosition = "0 0 0";  //skip
		kth2.LootContainerOrientation = "0 0 0";  //skip
		LootItem kth2_item1 = new LootItem("SVD", -1, 0.4);  // 40% chance to spawn SVD
		kth2_item1.Attachments.Insert(new LootItem("PSO1Optic", -1, 0.6));      // 60% chance
		kth2_item1.Attachments.Insert(new LootItem("SVD_Bipod", -1, 0.5));      // 50% chance
		kth2.LootItems.Insert(kth2_item1);
		LootItem kth2_item2 = new LootItem("Mosin9130", -1, 0.5);  // 50% chance to spawn Mosin
		kth2_item2.Attachments.Insert(new LootItem("PUScopeOptic", -1, 0.7));   // 70% chance
		kth2.LootItems.Insert(kth2_item2);
		kth2.BuildingSetFile = "koth_point_2_buildings.json";  // Use unique building set file for Point 2
		// Enemy Waves Configuration for kth2
		// Define capture progress triggers
		kth2.CaptureProgressTriggers.Insert(20);
		kth2.CaptureProgressTriggers.Insert(40);
		kth2.CaptureProgressTriggers.Insert(60);
		kth2.CaptureProgressTriggers.Insert(80);

		// Define enemy waves corresponding to the triggers

		// Wave 1
		WaveData wave1_kth2 = new WaveData();
		wave1_kth2.EntitiesToSpawn.Insert("ZmbM_CitizenBFat_Red");
		wave1_kth2.EntitiesToSpawn.Insert("ZmbM_FarmerFat_Brown");
		kth2.EnemyWaves.Insert(wave1_kth2);

		// Wave 2
		WaveData wave2_kth2 = new WaveData();
		wave2_kth2.EntitiesToSpawn.Insert("ZmbM_SoldierNormal");
		wave2_kth2.EntitiesToSpawn.Insert("ZmbF_SoldierNormal");
		kth2.EnemyWaves.Insert(wave2_kth2);

		// Wave 3
		WaveData wave3_kth2 = new WaveData();
		wave3_kth2.EntitiesToSpawn.Insert("ZmbM_FirefighterNormal");
		wave3_kth2.EntitiesToSpawn.Insert("ZmbF_FirefighterNormal");
		kth2.EnemyWaves.Insert(wave3_kth2);

		// Wave 4
		WaveData wave4_kth2 = new WaveData();
		wave4_kth2.EntitiesToSpawn.Insert("ZmbM_PolicemanSpecForce");
		wave4_kth2.EntitiesToSpawn.Insert("ZmbF_PoliceWomanNormal");
		kth2.EnemyWaves.Insert(wave4_kth2);
		
		cfg.KothDCfgList.Insert(kth2);

		SaveKothCFG();
		GenerateDefaultBuildingSetFile();
	}

	void ShuffleArray(ref array<int> arr)
	{
		for (int i = 0; i < arr.Count(); i++)
		{
			int randomIndex = Math.RandomInt(0, arr.Count());
			arr.SwapItems(i, randomIndex);
		}
	}


		
	void StartEvent()
	{
		// Declare nextEventTime only once at the beginning
		float nextEventTime;
		DeletePreviousLootContainer();

		// Ensure there is a delay between events
		if (kothlist.Count() >= cfg.MaxCountOfEvents)
		{
			//UtopiaLogger.Log("[KothConfig] Maximum number of KOTH events reached. Waiting before starting a new event.", "KOTH");
			nextEventTime = Math.RandomFloat(cfg.MinTimeBetweenEvents, cfg.MaxTimeBetweenEvents) * 1000;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StartEvent, nextEventTime, false);
			//UtopiaLogger.Log("[KothConfig] Next event scheduled in: " + nextEventTime + " ms", "KOTH");
			return;
		}

		players.Clear();
		GetGame().GetPlayers(players);
		int successCount = 0;

		if (players && players.Count() >= cfg.PlayerLimitToStart)
		{
			if (cfg.KothDCfgList.Count() > 0)
			{
				int remainingEvents = cfg.MaxCountOfEvents - kothlist.Count();
				if (remainingEvents > 0)
				{
					// Check if all locations have been used
					if (usedKothLocations.Count() >= cfg.KothDCfgList.Count())
					{
						usedKothLocations.Clear();  // Clear the list to reuse locations
						//UtopiaLogger.Log("[KothConfig] All KOTH locations used, resetting the list.", "KOTH");
					}

					// Create a list of available locations that haven't been used
					ref array<int> availableLocations = new array<int>;
					for (int i = 0; i < cfg.KothDCfgList.Count(); i++)
					{
						if (usedKothLocations.Find(i) == -1 && !IsLocationOccupied(cfg.KothDCfgList.Get(i).KothName))
						{
							availableLocations.Insert(i);
						}
					}

					// Shuffle available locations
					ShuffleArray(availableLocations);

					// Select available locations for the new event
					for (int j = 0; j < availableLocations.Count() && successCount < remainingEvents; j++)
					{
						int locationIndex = availableLocations[j];
						KothDList selectedLocation = cfg.KothDCfgList.Get(locationIndex);

						if (selectedLocation && selectedLocation.IsTriggerActive)
						{
							string posOri = selectedLocation.Cords;
							TStringArray loc = new TStringArray;
							posOri.Split("|", loc);
							string pos = loc.Get(0);
							string ori = loc.Get(1);

							KothPoint kothPoint = KothPoint.Cast(GetGame().CreateObject("KothPoint", pos.ToVector(), false, false, true));
							if (kothPoint)
							{
								kothPoint.SetOrientation(ori.ToVector());
								GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(kothPoint.SendKothData, 500, false, selectedLocation);
								lastKothPosition = pos.ToVector();
							}

							++successCount;

							// Mark the location as used
							usedKothLocations.Insert(locationIndex);
						}
					}
				}
			}
		}

		// Schedule the next event using the already declared nextEventTime variable
		nextEventTime = Math.RandomFloat(cfg.MinTimeBetweenEvents, cfg.MaxTimeBetweenEvents) * 1000;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StartEvent, nextEventTime, false);
		//UtopiaLogger.Log("[KothConfig] Next event scheduled in: " + nextEventTime + " ms", "KOTH");
	}


	void DeletePreviousLootContainer()
	{
		array<Object> nearbyObjects = new array<Object>;
		float radius = 100.0;  // Adjust as necessary

		if (lastKothPosition != vector.Zero)  // Ensure we have a valid position
		{
			GetGame().GetObjectsAtPosition3D(lastKothPosition, radius, nearbyObjects, null);
			foreach (Object obj : nearbyObjects)
			{
				// Check if it's the loot container
				if (obj && obj.GetType() == cfg.KothDCfgList.Get(0).LootContainerClass)
				{
					GetGame().ObjectDelete(obj);
					//UtopiaLogger.Log("[KothConfig] Previous loot container deleted.");
				}
			}
		}
	}





	// Helper function to check if a location is already occupied
	bool IsLocationOccupied(string kothName)
	{
		foreach (KothPoint kothPoint : kothlist)
		{
			if (kothPoint.kothdata.KothName == kothName)  // Check if the KOTH name is the same
			{
				return true;  // An event with this KOTH name is already active
			}
		}
		return false;
	}


	void CleanupExistingKOTHObjects()
	{
		//UtopiaLogger.Log("[KothConfig] CleanupExistingKOTHObjects called. Removing leftover KOTH-related objects.");

		array<Object> nearbyObjects = new array<Object>;
		float searchRadius = 1000.0;  // Search radius around the last known KOTH position. Adjust as necessary.

		// Ensure we have a valid position to search around
		if (lastKothPosition != vector.Zero)
		{
			GetGame().GetObjectsAtPosition3D(lastKothPosition, searchRadius, nearbyObjects, null);

			// Determine the class name of the configured loot container
			string lootContainerClassName = "";
			if (cfg && cfg.KothDCfgList.Count() > 0)
			{
				lootContainerClassName = cfg.KothDCfgList.Get(0).LootContainerClass;
			}

			// Iterate through all nearby objects and delete the relevant ones
			foreach (Object obj : nearbyObjects)
			{
				// Check if the object is a KothPoint, a configured loot container, or a part of KOTH building sets
				if (obj && (obj.IsKindOf("KothPoint") || obj.GetType() == lootContainerClassName || IsKothBuildingSetObject(obj)))
				{
					//UtopiaLogger.Log("[KothConfig] Deleting leftover object: " + obj.GetType() + " at position: " + obj.GetPosition().ToString());
					GetGame().ObjectDelete(obj);  // Delete the object from the game world
				}
			}

			//UtopiaLogger.Log("[KothConfig] CleanupExistingKOTHObjects completed.");
		}
		else
		{
			//UtopiaLogger.Log("[KothConfig] No valid last KOTH position found for cleanup.");
		}
	}


	// Helper function to identify if an object is part of the KOTH building set
	bool IsKothBuildingSetObject(Object obj)
	{
		// Get reference to the KothBuildingSetsLoader if initialized
		if (!g_KothConfig)
			return false;

		KothBuildingSetsLoader buildingLoader = g_KothConfig.GetCurrentBuildingLoader();

		// Check if the object is in the tracked list of buildings
		if (buildingLoader && buildingLoader.IsBuildingPartOfSet(obj))
		{
			return true;
		}

		return false;
	}

	// Add this function to your KothConfig class
	KothBuildingSetsLoader GetCurrentBuildingLoader()
	{
		if (g_KothConfig && g_KothConfig.cfg && g_KothConfig.m_BuildingLoader) 
		{
			return g_KothConfig.m_BuildingLoader;
		}
		return null;
	}
			
	void CorrectKothList(KothPoint kthpnt, bool add = true)
    {	
		if (!kothlist)
		{
			kothlist = new array<KothPoint>; 
		}
	
	    if(add)
		{
	       kothlist.Insert(kthpnt);
		}
        else
		{
           kothlist.RemoveItem(kthpnt);
		}		
	}	
		
	void SendKothInfo(PlayerBase player)
	{	
	    m_KothPos.Clear();
		m_Remainingtime.Clear();		

	    if(kothlist.Count() > 0)
	    {
			for (int i = 0; i < kothlist.Count(); i++)
		    {   
		          KothPoint kp = kothlist.Get(i);
				  if(kp)
				  {
					  m_KothPos.Insert(kp.GetPosition());
                      m_Remainingtime.Insert(kp.CaptureTriggerRemainingTime());
				  }
		    }
		}		
	}

};

static ref KothConfig g_KothConfig = new KothConfig();
static ref KothConfig GetKothConfig()
{
    if(!g_KothConfig) g_KothConfig = new KothConfig();
    return g_KothConfig;
};
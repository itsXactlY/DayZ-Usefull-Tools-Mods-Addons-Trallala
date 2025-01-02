class CTFLootItem
{
    string Name;    // Item classname
    float Chance;   // 0.0 to 1.0 (e.g., 0.5 for 50% chance)
    int Quantity;   // -1 indicates default or not set
    ref array<ref CTFLootItem> Attachments;

    void CTFLootItem(string name = "", float chance = 1.0, int qty = -1)
    {
        Name = name;
        Chance = chance;
        Quantity = qty;
        Attachments = new array<ref CTFLootItem>();
    }
}


class CTFLootPool
{
    string PoolName;                        // Name of the loot pool
    ref array<ref CTFLootItem> LootItems;   // Array of loot items

    void CTFLootPool(string poolName = "")
    {
        PoolName = poolName;
        LootItems = new array<ref CTFLootItem>();
    }
}


class CTFEventLocationConfig
{
    string LocationName;                         // Name of the location
    vector FlagSpawnPosition;                    // Position where the flag should spawn
    bool UseLootPool;                            // Whether to use the loot pool or not
    string SelectedLootPoolName;                 // The name of the loot pool to use (if enabled)
    ref array<ref CTFLootItem> RewardItems;      // Reward items for the location

    void CTFEventLocationConfig(string name = "", vector position = "0 0 0")//skip
    {
        LocationName = name;
        FlagSpawnPosition = position;
        UseLootPool = false;                     // Default: Not using loot pool
        SelectedLootPoolName = "DefaultPool";    // Default loot pool name

        RewardItems = new array<ref CTFLootItem>();

        // Example reward items with attachments
        CTFLootItem rewardItem1 = new CTFLootItem("MoneyRuble", 25000, 0.5); // 50% chance
        RewardItems.Insert(rewardItem1);

        CTFLootItem rewardItem2 = new CTFLootItem("Utopia_Bitcoin", 1, 1.0); // 100% chance
        RewardItems.Insert(rewardItem2);
    }

    // Method to get the specific loot pool by its name from the global loot pools
    ref CTFLootPool GetLootPoolByName(string poolName)
    {
        return GetCTFConfig().GetGlobalLootPoolByName(poolName);
    }
}


class CTFConfig {
    const static string ConfigDirectory = "$profile:\\Utopia_CTF";
    const static string ConfigFilePath = ConfigDirectory + "\\CTFConfig.json";

    float EventStartDelay;                       // Global event option: Time before event starts
    float TimeBetweenEvents;                     // Global event option: Time between events
    int MinPlayersRequired;                      // Minimum players required to start an event
    int MaxEvents;                               // Maximum number of concurrent events
    float FlagReturnTimeout;                     // Global flag return timeout duration
    string CaptureObjectClassname;               // Classname of the object to capture
    float MarkerUpdateInterval;
    ref array<ref CTFEventLocationConfig> EventLocations;  // Array to hold location-specific configurations
    ref array<ref CTFLootPool> GlobalLootPools;  // Array to hold global loot pools at the bottom of the config

    // Constructor for the CTFConfig class
    void CTFConfig() {
        EventStartDelay = 60.0;
        TimeBetweenEvents = 600.0;
        MinPlayersRequired = 1;                  // Default: Require at least 1 player to start an event
        MaxEvents = 1;
        FlagReturnTimeout = 300.0;               // Default flag return timeout (5 minutes)
        CaptureObjectClassname = "TerritoryFlag";
        MarkerUpdateInterval = 1.0;

        EventLocations = new array<ref CTFEventLocationConfig>;
        EventLocations.Insert(new CTFEventLocationConfig("City Center", "5000 0 5000"));//skip
        EventLocations.Insert(new CTFEventLocationConfig("Military Base", "7500 0 7500"));//skip
        EventLocations.Insert(new CTFEventLocationConfig("Airfield", "10000 0 10000"));//skip

        GlobalLootPools = new array<ref CTFLootPool>;
        ref CTFLootPool defaultPool = new CTFLootPool("DefaultPool");
        CTFLootItem item1 = new CTFLootItem("AKM", -1, 0.5);  // 50% chance to spawn
        item1.Attachments.Insert(new CTFLootItem("AK_Suppressor", -1, 0.5));  // 50% chance
        item1.Attachments.Insert(new CTFLootItem("PSO1Optic", -1, 0.7));      // 70% chance
        defaultPool.LootItems.Insert(item1);

        CTFLootItem item2 = new CTFLootItem("M4A1", -1, 0.3);  // 30% chance to spawn
        item2.Attachments.Insert(new CTFLootItem("M4_Suppressor", -1, 0.4));  // 40% chance
        defaultPool.LootItems.Insert(item2);

        // Add more items as needed...

        // Add the loot pool to the global loot pools
        GlobalLootPools.Insert(defaultPool);

        UtopiaLogger.Log("CTFConfig constructor called.", "CTF");
        LoadOrCreateConfig();
    }

    // Load or create the configuration file
    void LoadOrCreateConfig() {
        UtopiaLogger.Log("ConfigDirectory: " + ConfigDirectory, "CTF");
        UtopiaLogger.Log("ConfigFilePath: " + ConfigFilePath, "CTF");

        if (FileExist(ConfigFilePath)) {
            UtopiaLogger.Log("Configuration file found, loading it.", "CTF");
            JsonFileLoader<CTFConfig>.JsonLoadFile(ConfigFilePath, this);
        } else {
            UtopiaLogger.Log("Configuration file not found. Creating new configuration.", "CTF");
            CreateDefaultConfig();
        }
    }

    // Create the default configuration and save it to the file
    void CreateDefaultConfig() {
        if (!FileExist(ConfigDirectory)) {
            UtopiaLogger.Log("Directory does not exist. Attempting to create directory: " + ConfigDirectory, "CTF");
            bool created = MakeDirectory(ConfigDirectory);
            UtopiaLogger.Log("Directory creation status: " + created, "CTF");
        } else {
            UtopiaLogger.Log("Directory already exists: " + ConfigDirectory, "CTF");
        }

        UtopiaLogger.Log("Saving default configuration to: " + ConfigFilePath, "CTF");
        Save(ConfigFilePath);
    }

    // Save the configuration to a specified file path
    void Save(string filePath) {
        UtopiaLogger.Log("Attempting to save configuration to: " + filePath, "CTF");
        JsonFileLoader<CTFConfig>.JsonSaveFile(filePath, this);
        UtopiaLogger.Log("Configuration saved successfully.", "CTF");
    }

    ref CTFEventLocationConfig GetRandomEventLocationConfig() {
        if (EventLocations && EventLocations.Count() > 0) {
            int randomIndex = Math.RandomInt(0, EventLocations.Count());
            return EventLocations.Get(randomIndex);
        }
        return null;
    }

    ref CTFLootPool GetGlobalLootPoolByName(string poolName) {
        foreach (CTFLootPool pool : GlobalLootPools) {
            if (pool.PoolName == poolName) {
                return pool;
            }
        }
        return null; // Return null if not found
    }
}

// Global reference to CTFConfig for easy access
static ref CTFConfig g_CTFConfig;

// Get or create the global CTFConfig instance
static ref CTFConfig GetCTFConfig() {
    if (!g_CTFConfig) {
        g_CTFConfig = new CTFConfig();
    }
    return g_CTFConfig;
}

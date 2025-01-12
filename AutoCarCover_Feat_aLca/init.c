/**
 * Global storage for tracking car positions that have been processed
 * Maps string representations of position vectors to actual position vectors
 * Used to prevent redundant processing of cars in the same location
 */
ref map<string, vector> handledCarPositions = new map<string, vector>();

/**
 * Performs periodic checks for uncovered cars near players
 * Searches within a 150m radius of each player for vehicles requiring covers
 * Called every 60 seconds via the game's call queue system
 */
void CheckUncoveredCars()
{
    Print("aLca :: [InitC] Running sporadic uncovered car check near players...");
    
    array<Man> players = new array<Man>();
    GetGame().GetPlayers(players);

    foreach (Man player : players)
    {
        PlayerBase playerBase = PlayerBase.Cast(player);
        if (playerBase)
        {
            vector playerPos = playerBase.GetPosition();
            array<Object> nearbyObjects = new array<Object>();
            array<CargoBase> proxyCargos = new array<CargoBase>();

            GetGame().GetObjectsAtPosition3D(playerPos, 150.0, nearbyObjects, proxyCargos); // Check 150m radius

            foreach (Object obj : nearbyObjects)
            {
                CarScript car = CarScript.Cast(obj);
                if (car && car.IsAlive())
                {
                    if (!HasCarCoverNearby(car) && !HasPlayersInside(car))
                    {
                        Print("aLca :: [InitC] Found uncovered car at position: " + car.GetPosition());
                        car.H4_ServerCarCoverAfterLoad(); // Call the mod's cover function
                    }
                }
            }
        }
    }
}

/**
 * Checks if a vehicle has any players inside
 * @param car The CarScript instance to check for occupancy
 * @return bool Returns true if any player is occupying any seat in the vehicle
 */
bool HasPlayersInside(CarScript car)
{
    int crewSize = car.CrewSize();
    for (int i = 0; i < crewSize; i++)
    {
        if (car.CrewMember(i)) // Check if there's a player in this seat
        {
            return true;
        }
    }
    return false; // No players inside
}

/**
 * Checks for car covers near a vehicle and manages position tracking
 * @param car The CarScript instance to check for nearby covers
 * @return bool Returns true if a cover exists nearby or position was previously handled
 */
bool HasCarCoverNearby(CarScript car)
{
    vector carPos = car.GetPosition();
    string posKey = GetPositionKey(carPos);
    Print("aLca :: [HasCover] Checking car at position: " + carPos + " with key: " + posKey);

    if (handledCarPositions.Contains(posKey))
    {
         Print("aLca :: [HasCover] Position already handled: " + posKey);
         return true;
    }

    array<Object> nearbyObjects = new array<Object>();
    array<CargoBase> proxyCargos = new array<CargoBase>();
    
    GetGame().GetObjectsAtPosition3D(carPos, 2.0, nearbyObjects, proxyCargos);

    foreach (Object obj : nearbyObjects)
    {
        if (obj.IsKindOf("CarCoverBase"))
        {
            Print("aLca :: [HasCover] Found car cover, adding position: " + posKey);
            handledCarPositions.Insert(posKey, carPos);
            CarPositionStorage.SavePositions(handledCarPositions);
            return true;
        }
    }
    Print("aLca :: [HasCover] No car cover found.");
    return false;
}

/**
 * Creates a unique key for a vector
 * @param pos The position vector
 * @return string The unique key
 */
string GetPositionKey(vector pos)
{
    return pos[0].ToString() + "_" + pos[1].ToString() + "_" + pos[2].ToString();
}


/**
 * Initial server startup check for uncovered cars across the entire map
 * Called once during server initialization
 */
void CheckAllUncoveredCars()
{
    Print("aLca :: [InitC] Running global uncovered car check...");
    
    array<Object> allObjects = new array<Object>();
    GetGame().GetObjectsAtPosition("0 0 0", 50000.0, allObjects, null); // Check entire map

    foreach (Object obj : allObjects)
    {
        CarScript car = CarScript.Cast(obj);
        if (car && car.IsAlive())
        {
            if (!HasCarCoverNearby(car) && !HasPlayersInside(car))
            {
                Print("aLca :: [InitC] Found uncovered car at position: " + car.GetPosition());
                car.H4_ServerCarCoverAfterLoad();
            }
        }
    }
}

/**
 * Class to handle persistent storage of car positions
 */
class CarPositionStorage
{
    static const string STORAGE_PATH = "$profile:CarCover/CoveredCarPositions.json";
    
    /**
     * Saves positions to JSON file
     * @param positions Map of positions to save
     */
    static void SavePositions(map<string, vector> positions)
    {
        Print("aLca :: [Storage] Attempting to save " + positions.Count() + " car positions");
        JsonFileLoader<map<string, vector>>.JsonSaveFile(STORAGE_PATH, positions);
        Print("aLca :: [Storage] Saved " + positions.Count() + " car positions");
    }

    /**
     * Loads positions from JSON file
     * @return map<string, vector> Loaded positions or empty map if file doesn't exist
     */
    static map<string, vector> LoadPositions()
    {
        map<string, vector> positions = new map<string, vector>();
        Print("aLca :: [Storage] Attempting to load car positions");
        if (FileExist(STORAGE_PATH))
        {
            JsonFileLoader<map<string, vector>>.JsonLoadFile(STORAGE_PATH, positions);
            Print("aLca :: [Storage] Loaded " + positions.Count() + " car positions");
        }
        else
        {
            Print("aLca :: [Storage] No car position file found.");
        }
        return positions;
    }
        
        static void PrintStoragePath()
        {
                Print("aLca :: [Storage] Storage path: " + STORAGE_PATH);
        }
}

/**
 * Function to cover cars from loaded positions on startup
 */
void CoverCarsFromLoadedPositions()
{
    Print("aLca :: [InitC] Applying covers to loaded car positions...");
    foreach (string key, vector pos : handledCarPositions)
    {
        array<Object> allObjects = new array<Object>();
        GetGame().GetObjectsAtPosition(pos, 1.0, allObjects, null); // Check for car at the position
			
		foreach (Object obj : allObjects)
		{
			CarScript car = CarScript.Cast(obj);
			if (car && car.IsAlive())
			{
				Print("aLca :: [InitC] Applying cover to car at position: " + car.GetPosition());
				car.H4_ServerCarCoverAfterLoad();
			}
		}
    }
}

/**
 * Main initialization function for the server
 * Initializes economy, sets up periodic car checks, and manages date reset
 */
void main()
{
    // INIT ECONOMY--------------------------------------
    Hive ce = CreateHive();
    if (ce)
        ce.InitOffline();
        
    CarPositionStorage.PrintStoragePath();

    // Load saved positions
    handledCarPositions = CarPositionStorage.LoadPositions();
        
	// Apply covers to cars from loaded positions
	// GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CoverCarsFromLoadedPositions, 6000, false);

    // Run initial check for all cars on the map
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckAllUncoveredCars, 6000, false); // Run once after 5 seconds after startup in case of wiped server.
	GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CoverCarsFromLoadedPositions, 15000, false);

    // Set up periodic uncovered car check
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckUncoveredCars, 60000, true); // Every 60 seconds

    // DATE RESET AFTER ECONOMY INIT-------------------------
    int year, month, day, hour, minute;
    int reset_month = 9, reset_day = 20;
    GetGame().GetWorld().GetDate(year, month, day, hour, minute);

    if ((month == reset_month) && (day < reset_day))
    {
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    }
    else
    {
        if ((month == reset_month + 1) && (day > reset_day))
        {
            GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
        }
        else
        {
            if ((month < reset_month) || (month > reset_month + 1))
            {
                GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
            }
        }
    }
}

/**
 * Custom mission class extending MissionServer
 * Handles player spawning and initial equipment setup
 */
class CustomMission : MissionServer
{
    /**
     * Sets random health value for equipment items
     * @param itemEnt The EntityAI item to set health for
     */
    void SetRandomHealth(EntityAI itemEnt)
    {
        if (itemEnt)
        {
            float rndHlt = Math.RandomFloat(0.45, 0.65);
            itemEnt.SetHealth01("", "", rndHlt);
        }
    }

    /**
     * Creates a new player character
     * @param identity The player's identity
     * @param pos Spawn position vector
     * @param ctx Parameter read context
     * @param characterName Name of the character
     * @return PlayerBase The created player instance
     */
    override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
    {
        Entity playerEnt;
        playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
        Class.CastTo(m_player, playerEnt);

        GetGame().SelectPlayer(identity, m_player);

        return m_player;
    }

    /**
     * Sets up starting equipment for new players
     * Handles initial inventory, clothing, and quickbar items
     * @param player The PlayerBase instance to equip
     * @param clothesChosen Boolean indicating if clothes were already chosen
     */
    override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
    {
        EntityAI itemClothing;
        EntityAI itemEnt;
        ItemBase itemBs;
        float rand;

        itemClothing = player.FindAttachmentBySlotName("Body");
        if (itemClothing)
        {
            SetRandomHealth(itemClothing);

            itemEnt = itemClothing.GetInventory().CreateInInventory("BandageDressing");
            player.SetQuickBarEntityShortcut(itemEnt, 2);

            string chemlightArray[] = { "Chemlight_White", "Chemlight_Yellow", "Chemlight_Green", "Chemlight_Red" };
            int rndIndex = Math.RandomInt(0, 4);
            itemEnt = itemClothing.GetInventory().CreateInInventory(chemlightArray[rndIndex]);
            SetRandomHealth(itemEnt);
            player.SetQuickBarEntityShortcut(itemEnt, 1);

            rand = Math.RandomFloatInclusive(0.0, 1.0);
            if (rand < 0.35)
                itemEnt = player.GetInventory().CreateInInventory("Apple");
            else if (rand > 0.65)
                itemEnt = player.GetInventory().CreateInInventory("Pear");
            else
                itemEnt = player.GetInventory().CreateInInventory("Plum");
            player.SetQuickBarEntityShortcut(itemEnt, 3);
            SetRandomHealth(itemEnt);
        }

        itemClothing = player.FindAttachmentBySlotName("Legs");
        if (itemClothing)
            SetRandomHealth(itemClothing);

        itemClothing = player.FindAttachmentBySlotName("Feet");
    }
};

/**
 * Factory function for creating custom mission instance
 * @param path The mission path
 * @return Mission New CustomMission instance
 */
Mission CreateCustomMission(string path)
{
    return new CustomMission();
}

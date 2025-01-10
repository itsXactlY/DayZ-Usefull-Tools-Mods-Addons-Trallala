// Global map that stores the positions of covered cars to prevent re-handling
ref map<string, vector> handledCarPositions = new map<string, vector>();

/**
 * Checks for uncovered cars near all players in the game.
 * This function scans all players' positions and identifies uncovered cars nearby.
 * If a car has no cover nearby and no players inside, it is considered uncovered.
 */
void CheckUncoveredCars() {
    Print("aLca :: [InitC] Running sporadic uncovered car check near players...");
    array<Man> players = new array<Man>();
    GetGame().GetPlayers(players);

    // Iterate through each player in the game
    foreach(Man player: players) {
        PlayerBase playerBase = PlayerBase.Cast(player);
        if (playerBase) {
            vector playerPos = playerBase.GetPosition();
            array<Object> nearbyObjects = new array<Object>();
            array<CargoBase> proxyCargos = new array<CargoBase>();
            GetGame().GetObjectsAtPosition3D(playerPos, 150.0, nearbyObjects, proxyCargos);

            // Iterate through nearby objects and check for uncovered cars
            foreach(Object obj: nearbyObjects) {
                CarScript car = CarScript.Cast(obj);
                if (car && car.IsAlive()) {
                    if (!HasCarCoverNearby(car) && !HasPlayersInside(car)) {
                        Print("aLca :: [InitC] Found uncovered car at position: " + car.GetPosition());
                    }
                }
            }
        }
    }
}

/**
 * Checks if a car has any players inside.
 * @param car The car to check for passengers.
 * @return true if the car has any crew members, otherwise false.
 */
bool HasPlayersInside(CarScript car) {
    int crewSize = car.CrewSize();
    for (int i = 0; i < crewSize; i++) {
        if (car.CrewMember(i)) {
            return true;
        }
    }
    return false;
}

/**
 * Checks if a car has any cover nearby.
 * If the position has been handled already, it returns true to prevent redundant checks.
 * @param car The car to check for cover proximity.
 * @return true if a car cover is found near the car or if the position has already been handled, otherwise false.
 */
bool HasCarCoverNearby(CarScript car) {
    vector carPos = car.GetPosition();
    string posKey = GetPositionKey(carPos);

    Print("aLca :: [HasCover] Checking car at position: " + carPos + " with key: " + posKey);

    // Return true if the position has been handled before
    if (handledCarPositions.Contains(posKey)) {
        Print("aLca :: [HasCover] Position already handled: " + posKey);
        return true;
    }

    // Scan for nearby objects to check if there's a car cover
    array<Object> nearbyObjects = new array<Object>();
    array<CargoBase> proxyCargos = new array<CargoBase>();
    GetGame().GetObjectsAtPosition3D(carPos, 2.0, nearbyObjects, proxyCargos);

    foreach(Object obj: nearbyObjects) {
        if (obj.IsKindOf("CarCoverBase")) {
            Print("aLca :: [HasCover] Found car cover, adding position: " + posKey);
            handledCarPositions.Insert(posKey, carPos);
            CarPositionStorage.SavePositions(handledCarPositions);
            return true;
        }
    }
    return false;
}

/**
 * Generates a unique string key for a given position.
 * @param pos The 3D position of the car.
 * @return A unique string key in the format "x_y_z" representing the position.
 */
string GetPositionKey(vector pos) {
    return pos[0].ToString() + "_" + pos[1].ToString() + "_" + pos[2].ToString();
}

/**
 * Checks for all uncovered cars in the game world.
 * This function performs a global check and immediately applies cover to uncovered cars.
 */
void CheckAllUncoveredCars() {
    Print("aLca :: [InitC] Running global uncovered car check...");
    array<Object> allObjects = new array<Object>();
    GetGame().GetObjectsAtPosition("0 0 0", 50000.0, allObjects, null);

    // Iterate through all objects in the game
    foreach (Object obj : allObjects) {
        CarScript car = CarScript.Cast(obj);
        if (car && car.IsAlive()) {
            if (!HasCarCoverNearby(car) && !HasPlayersInside(car)) {
                Print("aLca :: [InitC] Found uncovered car at position: " + car.GetPosition());

                // Immediately cover the car and save its position
                string posKey = GetPositionKey(car.GetPosition());
                handledCarPositions.Insert(posKey, car.GetPosition());
                CarPositionStorage.SavePositions(handledCarPositions);
                car.H4_ServerCarCoverAfterLoad();
            }
        }
    }
}

/**
 * Handles the saving and loading of car positions to and from a file.
 * Used to keep track of which cars have been covered and prevent redundant actions.
 */
class CarPositionStorage {
    static const string STORAGE_PATH = "$profile:CarCover/CoveredCarPositions.json";

    /**
     * Saves the covered car positions to a file.
     * @param positions A map of car positions (as string keys and vector values).
     */
    static void SavePositions(map<string, vector> positions) {
        Print("aLca :: [Storage] Attempting to save " + positions.Count() + " car positions");
        JsonFileLoader<map<string, vector>>.JsonSaveFile(STORAGE_PATH, positions);
        Print("aLca :: [Storage] Saved " + positions.Count() + " car positions");
    }

    /**
     * Loads the covered car positions from a file.
     * @return A map of car positions loaded from storage.
     */
    static map<string, vector> LoadPositions() {
        map<string, vector> positions = new map<string, vector>();
        Print("aLca :: [Storage] Attempting to load car positions");
        if (FileExist(STORAGE_PATH)) {
            JsonFileLoader<map<string, vector>>.JsonLoadFile(STORAGE_PATH, positions);
            Print("aLca :: [Storage] Loaded " + positions.Count() + " car positions");
        } else {
            Print("aLca :: [Storage] No car position file found.");
        }
        return positions;
    }

    /**
     * Prints the storage path to the console for debugging purposes.
     */
    static void PrintStoragePath() {
        Print("aLca :: [Storage] Storage path: " + STORAGE_PATH);
    }
}

/**
 * Applies covers to cars based on loaded positions.
 * This function iterates over all stored positions and applies covers to cars at those positions.
 */
void CoverCarsFromLoadedPositions() {
    Print("aLca :: [InitC] Applying covers to loaded car positions...");
    foreach (string key, vector pos : handledCarPositions) {
        array<Object> allObjects = new array<Object>();
        GetGame().GetObjectsAtPosition(pos, 1.0, allObjects, null);
        foreach (Object obj : allObjects) {
            CarScript car = CarScript.Cast(obj);
            if (car && car.IsAlive()) {
                string posKey = GetPositionKey(car.GetPosition());
                if (!handledCarPositions.Contains(posKey)) { // Redundant Check, remove this
                    Print("aLca :: [InitC] Applying cover to car at position: " + car.GetPosition() + " (from loaded positions)");
                    car.H4_ServerCarCoverAfterLoad();
                } else {
                    Print("aLca :: [InitC] Car at position: " + car.GetPosition() + " already covered (from loaded positions)");
                }
            }
        }
    }
}

/**
 * Main entry point for the server-side mission initialization.
 * It handles loading of car positions, setting up periodic checks, and resetting the game date if necessary.
 */
void main() {
    Hive ce = CreateHive();
    if (ce) ce.InitOffline();
    CarPositionStorage.PrintStoragePath();
    handledCarPositions = CarPositionStorage.LoadPositions();

    // Check for cars and apply covers at startup
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(EnsureCoveredCarsAtStartup, 5000, false);

    // Set periodic checks
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckAllUncoveredCars, 15000, false);
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckUncoveredCars, 60000, true);

    // Handle game world date reset logic
    ResetGameDate();
}

/**
 * Ensures all currently loaded cars are covered or marked as handled at startup.
 */
void EnsureCoveredCarsAtStartup() {
    Print("aLca :: [InitC] Ensuring covered cars at startup...");

    array<Object> allObjects = new array<Object>();
    GetGame().GetObjectsAtPosition("0 0 0", 50000.0, allObjects, null);

    foreach (Object obj : allObjects) {
        CarScript car = CarScript.Cast(obj);
        if (car && car.IsAlive()) {
            string posKey = GetPositionKey(car.GetPosition());

            // Check if this car already has a cover
            if (HasCarCoverNearby(car)) {
                Print("aLca :: [InitC] Car at position: " + car.GetPosition() + " already covered.");
                handledCarPositions.Insert(posKey, car.GetPosition());
                CarPositionStorage.SavePositions(handledCarPositions);
                continue;
            }

            // If car is not already handled, apply cover and flag it
            if (!handledCarPositions.Contains(posKey)) {
                Print("aLca :: [InitC] Covering uncovered car at position: " + car.GetPosition());
                handledCarPositions.Insert(posKey, car.GetPosition());
                CarPositionStorage.SavePositions(handledCarPositions);
                car.H4_ServerCarCoverAfterLoad();
            }
        }
    }
}

/**
 * Resets the game date to prevent time inconsistencies during gameplay.
 */
void ResetGameDate() {
    int year, month, day, hour, minute;
    int reset_month = 9, reset_day = 20;
    GetGame().GetWorld().GetDate(year, month, day, hour, minute);

    if ((month == reset_month) && (day < reset_day)) {
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    } else if ((month == reset_month + 1) && (day > reset_day)) {
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    } else if ((month < reset_month) || (month > reset_month + 1)) {
        GetGame().GetWorld().SetDate(year, reset_month, reset_day, hour, minute);
    }
}

/**
 * Custom mission class that overrides the default mission behavior for player creation and item equip setup.
 */
class CustomMission: MissionServer {

    /**
     * Sets a random health value for an item.
     * @param itemEnt The item whose health is to be set.
     */
    void SetRandomHealth(EntityAI itemEnt) {
        if (itemEnt) {
            float rndHlt = Math.RandomFloat(0.45, 0.65);
            itemEnt.SetHealth01("", "", rndHlt);
        }
    }

    /**
     * Creates a custom character for the player at the given position.
     * @param identity The player's identity.
     * @param pos The spawn position of the character.
     * @param ctx The parameters of the character (usually for customization).
     * @param characterName The name of the character.
     * @return The created PlayerBase instance.
     */
    override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName) {
        Entity playerEnt;
        playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
        Class.CastTo(m_player, playerEnt);
        GetGame().SelectPlayer(identity, m_player);
        return m_player;
    }

    /**
     * Initializes the starting equipment for the player, including health and inventory items.
     * @param player The player to equip.
     * @param clothesChosen Whether the player has chosen their clothes.
     */
    override void StartingEquipSetup(PlayerBase player, bool clothesChosen) {
        EntityAI itemClothing;
        EntityAI itemEnt;
        float rand;
        itemClothing = player.FindAttachmentBySlotName("Body");
        if (itemClothing) {
            SetRandomHealth(itemClothing);
            itemEnt = itemClothing.GetInventory().CreateInInventory("BandageDressing");
            player.SetQuickBarEntityShortcut(itemEnt, 2);
            string chemlightArray[] = {"Chemlight_White", "Chemlight_Yellow", "Chemlight_Green", "Chemlight_Red"};
            int rndIndex = Math.RandomInt(0, 4);
            itemEnt = itemClothing.GetInventory().CreateInInventory(chemlightArray[rndIndex]);
            SetRandomHealth(itemEnt);
            player.SetQuickBarEntityShortcut(itemEnt, 1);
            rand = Math.RandomFloatInclusive(0.0, 1.0);
            if (rand < 0.35) itemEnt = player.GetInventory().CreateInInventory("Apple");
            else if (rand > 0.65) itemEnt = player.GetInventory().CreateInInventory("Pear");
            else itemEnt = player.GetInventory().CreateInInventory("Plum");
            player.SetQuickBarEntityShortcut(itemEnt, 3);
            SetRandomHealth(itemEnt);
        }
        itemClothing = player.FindAttachmentBySlotName("Legs");
        if (itemClothing) SetRandomHealth(itemClothing);
        itemClothing = player.FindAttachmentBySlotName("Feet");
    }
};

/**
 * Creates a custom mission with the provided path.
 * @param path The path to the mission's configuration.
 * @return The created CustomMission instance.
 */
Mission CreateCustomMission(string path) {
    return new CustomMission();
}

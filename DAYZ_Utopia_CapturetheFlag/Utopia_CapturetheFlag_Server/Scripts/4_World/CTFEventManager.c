class CTFEventManager {
    private ref CTFConfig m_Config;             // Configuration object
    private Object m_FlagBase;                  // Reference to the base flag object (Utopia_Flag_Base)
    private Object m_CaptureFlag;               // Reference to the detached Utopia_CaptureFlag
    private PlayerBase m_FlagHolder;            // Reference to the player holding the flag
    private string currentLootPoolName;         // Variable to store the current loot pool name
    private bool m_EventInProgress = false;     // Flag to check if an event is in progress
    private static int s_ActiveEvents = 0;      // Static counter for active events
    ref CTFEventLocationConfig m_CurrentLocation; // Holds the configuration of the current event location
    private int m_PlayerCheckInterval = 60000;  // Interval in milliseconds to check player count (10 seconds)
    private const float FLAG_RETURN_TIMEOUT = 300.0; // 5 minutes default
    private bool m_IsFlagInitiallyPlaced = false;  // Tracks whether the flag is in its initial placement state
    private bool m_IsFlagDropped = false;       // Tracks if the flag was dropped
    private ref Timer m_FlagReturnTimer;        // Timer to return the flag to base
    #ifdef BASICMAP
    private BasicMapMarker m_FlagMarker;        // Reference to the map marker for the flag
    #endif
    #ifdef LBmaster_Groups
    private int m_LBFlagMarkerId = -1;          // ID for the LB marker tracking the flag
    #endif
    private ref CTFTickScheduler m_TickScheduler; // Tick scheduler for marker updates

    void CTFEventManager() {
        m_Config = GetCTFConfig();
        UtopiaLogger.Log("CTF Event Manager initialized.", "CTF");

        m_FlagReturnTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
        m_TickScheduler = new CTFTickScheduler(this, m_Config.MarkerUpdateInterval);
        MonitorPlayerCount();
    }

    void StartMarkerUpdateScheduler()
    {
        if (m_TickScheduler)
        {
            m_TickScheduler.SetInterval(m_Config.MarkerUpdateInterval);  // Update the interval dynamically based on config
            m_TickScheduler.Start();
        }
    }


    void MonitorPlayerCount() {
        UtopiaLogger.Log("Checking player count to meet the minimum required players.", "CTF");
        int onlinePlayersCount = GetNumberOfOnlinePlayers();
        UtopiaLogger.Log("Current online players: " + onlinePlayersCount, "CTF");
        UtopiaLogger.Log("Minimum required players to start: " + m_Config.MinPlayersRequired, "CTF");

        if (onlinePlayersCount >= m_Config.MinPlayersRequired) {
            UtopiaLogger.Log("Minimum player requirement met. Starting CTF Event.", "CTF");
            StartEvent(); // Start the event if requirement is met
        } else {
            UtopiaLogger.Log("Not enough players online to start the event. Minimum required: " + m_Config.MinPlayersRequired, "CTF");
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MonitorPlayerCount, m_PlayerCheckInterval, false);
        }
    }

    // Start the CTF event
    void StartEvent() {
        if (m_EventInProgress) {
            UtopiaLogger.Log("Event is already in progress. Ignoring StartEvent call.", "CTF");
            return;
        }

        int onlinePlayersCount = GetNumberOfOnlinePlayers();
        UtopiaLogger.Log("Current online players: " + onlinePlayersCount, "CTF");
        UtopiaLogger.Log("Minimum required players to start: " + m_Config.MinPlayersRequired, "CTF");

        if (onlinePlayersCount < m_Config.MinPlayersRequired) {
            UtopiaLogger.Log("Not enough players online to start the event. Minimum required: " + m_Config.MinPlayersRequired, "CTF");
            return;
        }

        UtopiaLogger.Log("Starting CTF Event in " + m_Config.EventStartDelay + " seconds.", "CTF");
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BeginEvent, m_Config.EventStartDelay * 1000, false);
    }

    // Begin the CTF event and spawn the flag
    void BeginEvent() 
    {
        if (m_EventInProgress) {
            UtopiaLogger.Log("[CTFEventManager] Event is already in progress. Cannot begin another event.", "CTF");
            return;
        }

        m_EventInProgress = true;
        UtopiaLogger.Log("CTF Event has begun. Flag will be spawned at the specified location.", "CTF");
        SendNotification("The Capture the Flag event has started! Find the flag and bring it back to the designated location.");

        m_CurrentLocation = m_Config.GetRandomEventLocationConfig();
        if (!m_CurrentLocation) {
            UtopiaLogger.Log("[CTFEventManager] No event locations available. Ending event.", "CTF");
            EndEvent(null);
            return;
        }

        // Spawn the Utopia_Flag_Base at the configured position using Entity AI
        vector flagPosition = m_CurrentLocation.FlagSpawnPosition;
        m_FlagBase = SpawnEntityAI("Utopia_Flag_Base", flagPosition, "0 0 0");//skip
        UtopiaLogger.Log("[CTFEventManager] Spawned flag base at position: " + flagPosition.ToString(), "CTF");

        if (m_FlagBase) {
            EntityAI flagEntityAI = EntityAI.Cast(m_FlagBase);
            if (flagEntityAI) {
                // Attach the Utopia_CaptureFlag item to the Utopia_Flag_Base and store its reference
                m_CaptureFlag = flagEntityAI.GetInventory().CreateAttachment("Utopia_CaptureFlag");
                if (m_CaptureFlag) {
                    UtopiaLogger.Log("[CTFEventManager] Successfully attached and tracked the capture flag.", "CTF");

                    // Set initial placement state
                    m_IsFlagInitiallyPlaced = true;  // Mark the flag as in its initial placement state
                } else {
                    UtopiaLogger.Log("[CTFEventManager] Failed to attach the capture flag to the flag base.", "CTF");
                }
            } else {
                UtopiaLogger.Log("[CTFEventManager] Failed to cast the flag base to EntityAI.", "CTF");
            }
        } else {
            UtopiaLogger.Log("[CTFEventManager] Failed to spawn the flag base. Ending event.", "CTF");
            EndEvent(null);
            return;
        }

        #ifdef BASICMAP
        CreateOrUpdateFlagMarker("Flag Location", flagPosition);
        #endif

        #ifdef LBmaster_Groups
        m_LBFlagMarkerId = UtopiaMarkerManager.AddLBMarker("CTF Flag", flagPosition, UtopiaMarkerManager.CTF_MARKER_ICON, {0, 255, 0});
        if (m_LBFlagMarkerId != -1) {
            UtopiaLogger.Log("[CTFEventManager] Created LB marker for flag with ID: " + m_LBFlagMarkerId, "CTF");
        }
        #endif

        m_TickScheduler.Start();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MonitorEventProgress, 1000, true);
    }


    int GetNumberOfOnlinePlayers() {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        return players.Count();
    }

    // Monitor the progress of the event
    void MonitorEventProgress()
    {
        if (!m_CaptureFlag)
        {
            UtopiaLogger.Log("[CTFEventManager] Capture flag no longer exists. Ending event.", "CTF");
            EndEvent(null);
            return;
        }

        // Ensure the flag is not marked as dropped when it is attached to the base or in its initial placement state
        EntityAI parent = EntityAI.Cast(m_CaptureFlag.GetParent());
        Utopia_Flag_Base flagBase;
        if (Class.CastTo(flagBase, parent))
        {
            // Skip drop detection if the flag is in its initial placement state
            if (m_IsFlagInitiallyPlaced)
            {
                m_IsFlagDropped = false;
                UtopiaLogger.Log("[CTFEventManager] Flag is attached to the base and in initial placement. Not considering it as dropped.", "CTF");
                m_IsFlagInitiallyPlaced = false;  // Mark initial placement as complete
                return;
            }
        }

        if (m_FlagHolder)
        {
            // Check if the player holding the flag is still alive, otherwise drop the flag at their death position
            if (!m_FlagHolder.IsAlive())
            {
                OnPlayerDeath(m_FlagHolder);
                return;
            }

            if (!IsPlayerHoldingFlag(m_FlagHolder))
            {
                SendNotification("The flag has been dropped!");
                m_FlagHolder = null;
                m_IsFlagDropped = true;
                StartFlagReturnCountdown();
                UpdateFlagMarker();
                return;
            }
            if (CheckIfPlayerInCaptureZone(m_FlagHolder))
            {
                EndEvent(m_FlagHolder);
            }
        }
        else
        {
            CheckForFlagPickup(); // Check if any player picked up the flag
        }
    }




    // Check if any player has picked up the flag
    void CheckForFlagPickup() 
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);

        bool isFlagPickedUp = false;

        foreach (Man player : players) {
            PlayerBase pb = PlayerBase.Cast(player);
            if (pb && pb.IsAlive() && IsPlayerHoldingFlag(pb)) {
                m_FlagHolder = pb;
                SendNotification(pb.GetIdentity().GetName() + " has picked up the flag! Stop them before they reach the designated location.");
                UtopiaLogger.Log("Player " + pb.GetIdentity().GetName() + " has picked up the flag.", "CTF");

                UpdateFlagMarker();
                m_TickScheduler.Start();  
                isFlagPickedUp = true;
                m_IsFlagDropped = false;  
                m_FlagReturnTimer.Stop();
                return;
            }
        }

        if (!isFlagPickedUp && !m_IsFlagDropped) {
            SendNotification("The flag has been dropped!");
            m_IsFlagDropped = true;
            StartFlagReturnCountdown();
            m_TickScheduler.Start();
        }
    }

    // Handle flag drop event
    void HandleFlagDropped() 
    {
        string flagDropperName;

        if (m_FlagHolder && m_FlagHolder.GetIdentity()) {
            flagDropperName = m_FlagHolder.GetIdentity().GetName();
        } else {
            flagDropperName = "unknown";
        }

        UtopiaLogger.Log("Flag has been dropped by: " + flagDropperName, "CTF");

        UtopiaLogger.Log("Flag has been dropped by: " + flagDropperName, "CTF");
        SendNotification("The flag has been dropped!");

        m_IsFlagDropped = true;
        m_FlagHolder = null;

        StartFlagReturnCountdown();
    }

    void OnPlayerDeath(PlayerBase player)
    {
        if (player && player == m_FlagHolder)
        {
            // Drop the flag at the player's death position
            vector deathPosition = player.GetPosition();
            UtopiaLogger.Log("[CTFEventManager] Player holding the flag has died. Dropping the flag at their death position: " + deathPosition.ToString(), "CTF");
            
            m_CaptureFlag.SetPosition(deathPosition);  // Drop the flag at the player's death position
            m_IsFlagDropped = true;
            m_FlagHolder = null;  // Remove reference to the player holding the flag

            SendNotification("The flag has been dropped at " + player.GetIdentity().GetName() + "'s death location!");
            StartFlagReturnCountdown();
            UpdateFlagMarker();  // Update the flag marker to reflect the new position
        }
    }



    // Start the countdown to return the flag to base
    void StartFlagReturnCountdown() 
    {
        UtopiaLogger.Log("Starting flag return countdown. Initial Placement State: " + m_IsFlagInitiallyPlaced.ToString(), "CTF");
        m_FlagReturnTimer.Run(FLAG_RETURN_TIMEOUT, this, "ReturnFlagToBase");
    }


    // Return flag to base after timeout
    void ReturnFlagToBase() 
    {
        if (!m_FlagHolder) { 
            UtopiaLogger.Log("Flag has returned to the base.", "CTF");
            SendNotification("The flag has returned to its base!");
            if (m_CaptureFlag) {
                m_CaptureFlag.SetPosition(m_CurrentLocation.FlagSpawnPosition);
                UpdateFlagMarkerToPosition(m_CurrentLocation.FlagSpawnPosition);
            }
            m_IsFlagDropped = false;
        }
    }

    // Update marker based on flag's current position
    void UpdateFlagMarkerToPosition(vector flagPosition) {
        #ifdef BASICMAP
        CreateOrUpdateFlagMarker("Flag Location", flagPosition);
        #endif

        #ifdef LBmaster_Groups
        UtopiaMarkerManager.UpdateCTFMarkerPosition(m_LBFlagMarkerId, flagPosition);
        #endif
    }

    // Update the flag marker
    void UpdateFlagMarker() {
        vector currentPosition;
        if (m_FlagHolder) {
            currentPosition = m_FlagHolder.GetPosition();
            UtopiaLogger.Log("[CTFEventManager] Flag is being held by: " + m_FlagHolder.GetIdentity().GetName() + ". Updating marker to their position: " + currentPosition.ToString(), "CTF");
        } else if (m_CaptureFlag) {
            currentPosition = m_CaptureFlag.GetPosition();
            UtopiaLogger.Log("[CTFEventManager] Flag is on the ground. Updating marker to flag position: " + currentPosition.ToString(), "CTF");
        } else {
            UtopiaLogger.Log("[CTFEventManager] Flag or flag holder not found. Cannot update marker.", "CTF");
            return;
        }

        #ifdef BASICMAP
        CreateOrUpdateFlagMarker("Capture the Flag", currentPosition);
        #endif

        #ifdef LBmaster_Groups
        if (m_LBFlagMarkerId != -1) {
            UtopiaMarkerManager.UpdateCTFMarkerPosition(m_LBFlagMarkerId, currentPosition);
            UtopiaLogger.Log("[CTFEventManager] CTF LB marker updated to position: " + currentPosition.ToString(), "CTF");
        }
        #endif
    }

    bool IsPlayerHoldingFlag(PlayerBase player) {
        if (player && player.GetInventory()) {
            array<EntityAI> attachments = new array<EntityAI>();
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, attachments);
            foreach (EntityAI attachment : attachments) {
                if (attachment && (attachment.GetType() == "Utopia_CaptureFlag")) {
                    return true;
                }
            }
        }
        return false;
    }

    bool CheckIfPlayerInCaptureZone(PlayerBase player) {
        array<Object> objectsNearby = new array<Object>();
        GetGame().GetObjectsAtPosition(player.GetPosition(), 5.0, objectsNearby, null);

        foreach (Object obj : objectsNearby) {
            if (obj && obj.GetType() == m_Config.CaptureObjectClassname) {
                Print("[CTFEventManager] Player is in the capture zone.");
                return true;
            }
        }
        return false;
    }

    void EndEvent(PlayerBase winningPlayer) {
        if (winningPlayer) {
            SendNotification(winningPlayer.GetIdentity().GetName() + " has captured the flag and won the event!");
            SpawnRewards(winningPlayer.GetPosition());
        } else {
            SendNotification("The Capture the Flag event has ended with no winner.");
        }

        RemoveMarkers();
        m_TickScheduler.Stop();
        Cleanup();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MonitorEventProgress);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StartEvent, m_Config.TimeBetweenEvents * 1000, false);
    }

    // Custom function to determine if an item is stackable
    bool IsItemStackable(Object item)
    {
        ItemBase stackableItem;
        Magazine mag;

        // Check if the item is a magazine first
        if (Class.CastTo(mag, item))
        {
            UtopiaLogger.Log("[CTFEventManager] Item identified as Magazine: " + item.GetType(), "CTF");
            return mag.GetAmmoMax() > 1;  // Magazines can be considered stackable if they hold more than one round
        }

        // Check if the item can be cast to the ItemBase type and has quantity attributes
        if (Class.CastTo(stackableItem, item))
        {
            UtopiaLogger.Log("[CTFEventManager] Item identified as ItemBase: " + item.GetType(), "CTF");

            // Check if the item has quantity or maximum quantity greater than 1
            if (stackableItem.HasQuantity() || stackableItem.GetQuantityMax() > 1)
            {
                return true;  // The item is stackable based on its attributes
            }
        }

        UtopiaLogger.Log("[CTFEventManager] Item is not stackable: " + item.GetType(), "CTF");
        return false;  // Item is not stackable if it doesn't fit the criteria above
    }


    static ItemBase SetQuantity(ItemBase item, int quantity)
    {
        if (!item)
        {
            UtopiaLogger.Log("[CTFEventManager] SetQuantity: item is null, cannot set quantity.", "CTF");
            return null;
        }

        Magazine mag = Magazine.Cast(item);
        if (mag)
        {
            mag.ServerSetAmmoCount(quantity); // Set ammo count for magazines
            UtopiaLogger.Log("[CTFEventManager] Ammo count set for magazine: " + item.GetType() + " to " + quantity, "CTF");
        }
        else
        {
            item.SetQuantity(quantity, true); // Set quantity for other stackable items
            UtopiaLogger.Log("[CTFEventManager] Quantity set for stackable item: " + item.GetType() + " to " + quantity, "CTF");
        }

        return item;
    }


    Object SpawnEntityAI(string classname, vector position, vector orientation, int quantity = -1)
    {
        Object entityAI = GetGame().CreateObjectEx(classname, position, ECE_INITAI | ECE_CREATEPHYSICS | ECE_PLACE_ON_SURFACE);
        
        if (entityAI)
        {
            entityAI.SetOrientation(orientation);  // Set initial orientation
            entityAI.SetPosition(position);        // Re-set position to make sure it's accurate
            UtopiaLogger.Log("[CTFEventManager] Spawned Entity AI: " + classname + " at position: " + position.ToString(), "CTF");

            // Ensure quantity is set correctly for stackable items
            ItemBase stackableItem = ItemBase.Cast(entityAI);
            if (stackableItem && quantity > 0)
            {
                SetQuantity(stackableItem, quantity);
            }
            else
            {
                UtopiaLogger.Log("[CTFEventManager] Item is not stackable or quantity is invalid: " + classname, "CTF");
            }
        }
        else
        {
            UtopiaLogger.Log("[CTFEventManager] Failed to spawn Entity AI: " + classname, "CTF");
        }

        return entityAI;
    }

    // Modified SpawnRewards function without `?` operator
    void SpawnRewards(vector spawnPosition)
    {
        UtopiaLogger.Log("[SpawnRewards] Called with spawnPosition: " + spawnPosition.ToString(), "CTF");

        ref CTFLootPool selectedLootPool = null;

        if (m_CurrentLocation.UseLootPool)
        {
            selectedLootPool = m_CurrentLocation.GetLootPoolByName(m_CurrentLocation.SelectedLootPoolName);
            UtopiaLogger.Log("[SpawnRewards] Using loot pool: " + m_CurrentLocation.SelectedLootPoolName, "CTF");
        }
        else
        {
            // Create a temporary loot pool from the reward items
            selectedLootPool = new CTFLootPool("TemporaryRewardPool");
            foreach (CTFLootItem rewardItem : m_CurrentLocation.RewardItems)
            {
                selectedLootPool.LootItems.Insert(rewardItem);
            }
            UtopiaLogger.Log("[SpawnRewards] Using temporary loot pool from reward items.", "CTF");
        }

        if (selectedLootPool)
        {
            foreach (CTFLootItem lootItem : selectedLootPool.LootItems)
            {
                UtopiaLogger.Log("[SpawnRewards] Attempting to spawn loot item: " + lootItem.Name, "CTF");
                SpawnItemWithAttachments(spawnPosition, lootItem);
            }
        }
        else
        {
            UtopiaLogger.Log("[CTFEventManager] No loot pool or reward items found. No rewards spawned.", "CTF");
        }
    }



    void SpawnItemWithAttachments(vector position, CTFLootItem lootItem)
    {
        UtopiaLogger.Log("[SpawnItemWithAttachments] Called for item: " + lootItem.Name + " with chance: " + lootItem.Chance, "CTF");

        if (Math.RandomFloat01() <= lootItem.Chance)
        {
            EntityAI itemEntity = EntityAI.Cast(GetGame().CreateObject(lootItem.Name, position));

            if (!itemEntity)
            {
                UtopiaLogger.Log("[CTFEventManager] Failed to create item: " + lootItem.Name, "CTF");
                return;
            }

            UtopiaLogger.Log("[SpawnItemWithAttachments] Spawned item: " + lootItem.Name + " at position: " + position.ToString(), "CTF");

            ItemBase item = ItemBase.Cast(itemEntity);

            // Set quantity if specified
            if (lootItem.Quantity != -1 && item)
            {
                SetItemQuantity(item, lootItem.Quantity);
                UtopiaLogger.Log("[SpawnItemWithAttachments] Set quantity: " + lootItem.Quantity + " for item: " + lootItem.Name, "CTF");
            }

            // Handle attachments recursively
            if (lootItem.Attachments && lootItem.Attachments.Count() > 0)
            {
                UtopiaLogger.Log("[SpawnItemWithAttachments] Spawning attachments for item: " + lootItem.Name, "CTF");
                foreach (CTFLootItem attachmentData : lootItem.Attachments)
                {
                    SpawnAttachment(itemEntity, attachmentData);
                }
            }
        }
        else
        {
            UtopiaLogger.Log("[SpawnItemWithAttachments] Item not spawned due to chance: " + lootItem.Chance, "CTF");
        }
    }


    void SpawnAttachment(EntityAI parent, CTFLootItem attachmentData)
    {
        if (Math.RandomFloat01() <= attachmentData.Chance)
        {
            EntityAI attachmentEntity = parent.GetInventory().CreateAttachment(attachmentData.Name);

            if (!attachmentEntity)
            {
                UtopiaLogger.Log("[CTFEventManager] Failed to attach item: " + attachmentData.Name + " to " + parent.GetType(), "CTF");
                return;
            }

            ItemBase attachmentItem = ItemBase.Cast(attachmentEntity);

            // Set quantity if specified
            if (attachmentData.Quantity != -1 && attachmentItem)
            {
                SetItemQuantity(attachmentItem, attachmentData.Quantity);
            }

            // Handle nested attachments
            if (attachmentData.Attachments && attachmentData.Attachments.Count() > 0)
            {
                foreach (CTFLootItem nestedAttachment : attachmentData.Attachments)
                {
                    SpawnAttachment(attachmentEntity, nestedAttachment);
                }
            }
        }
    }

    // Helper method to set quantity
    void SetItemQuantity(ItemBase item, int quantity)
    {
        Magazine mag = Magazine.Cast(item);
        if (mag)
            mag.ServerSetAmmoCount(quantity);
        else
            item.SetQuantity(quantity);
    }





    void RemoveMarkers() {
        #ifdef BASICMAP
        RemoveFlagMarker();
        #endif

        #ifdef LBmaster_Groups
        if (m_LBFlagMarkerId != -1) {
            UtopiaMarkerManager.RemoveLBMarker(m_LBFlagMarkerId);
            m_LBFlagMarkerId = -1;
        }
        #endif
    }

    void Cleanup() 
    {
    // Check and delete the Utopia_CaptureFlag object if it exists
        if (m_CaptureFlag) {
            GetGame().ObjectDelete(m_CaptureFlag);
            UtopiaLogger.Log("[CTFEventManager] Utopia_CaptureFlag object deleted successfully.", "CTF");
            m_CaptureFlag = null;  // Reset the reference to null
        }

        // Check and delete the Utopia_Flag_Base object if it exists
        if (m_FlagBase) {
            GetGame().ObjectDelete(m_FlagBase);
            UtopiaLogger.Log("[CTFEventManager] Utopia_Flag_Base object deleted successfully.", "CTF");
            m_FlagBase = null;  // Reset the reference to null
        }

        // Remove any markers related to the event
        RemoveMarkers();

        // Reset all event-related flags and variables
        m_FlagHolder = null;
        m_EventInProgress = false;
        m_IsFlagDropped = false;

        // Stop the flag return timer if it's running
        if (m_FlagReturnTimer && m_FlagReturnTimer.IsRunning()) {
            m_FlagReturnTimer.Stop();
            UtopiaLogger.Log("[CTFEventManager] Flag return timer stopped during cleanup.", "CTF");
        }

        // Stop the tick scheduler if it's active
        if (m_TickScheduler) {
            m_TickScheduler.Stop();
            UtopiaLogger.Log("[CTFEventManager] Tick scheduler stopped during cleanup.", "CTF");
        }

        // Remove any ongoing progress monitoring
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MonitorEventProgress);

        UtopiaLogger.Log("[CTFEventManager] Cleanup completed. Event state reset.", "CTF");
    }


    void SendNotification(string message) {
        NotificationSystem.SendNotificationToPlayerIdentityExtended(null, 5, message);
        Print("[CTFEventManager] " + message);
    }

    #ifdef BASICMAP
    // Create or update the flag marker on the map
    void CreateOrUpdateFlagMarker(string name, vector position) {
        if (!m_FlagMarker) {
            m_FlagMarker = BasicMapController.GetInstance().CreateCTFServerMarker(name, position);
            UtopiaLogger.Log("[CTFEventManager] Created new flag marker at position: " + position.ToString(), "CTF");
        } else {
            BasicMapController.GetInstance().UpdateMarkerPosition(m_FlagMarker, position);
            UtopiaLogger.Log("[CTFEventManager] Updated flag marker to position: " + position.ToString(), "CTF");
        }
    }

    // Remove the flag marker from the map
    void RemoveFlagMarker() {
        if (m_FlagMarker) {
            BasicMapController.GetInstance().CTFRemoveMarker(m_FlagMarker);
            m_FlagMarker = null;
            Print("[CTFEventManager] Removed flag marker from the map.");
        }
    }
    #endif
}

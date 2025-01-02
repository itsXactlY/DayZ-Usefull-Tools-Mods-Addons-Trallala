class KothBuildingSetsLoader
{
    autoptr array<Object> m_Builds;
    const string m_BuildingFiles_Dir = "$profile:Utopia_Koth/Objects/";

    void KothBuildingSetsLoader()
    {
        m_Builds = new array<Object>;
    }

    void ReadFileAndSpawnBuildings(string filename)
    {
        string filePath = m_BuildingFiles_Dir + filename;

        if (FileExist(filePath))
        {
            ref array<ref KothBuilding> buildings = new array<ref KothBuilding>;

            // Load the JSON file into the array of buildings
            JsonFileLoader<array<ref KothBuilding>>.JsonLoadFile(filePath, buildings);

            // Iterate through the buildings and spawn them
            foreach (KothBuilding building : buildings)
            {
                SpawnKothBuildingObject(building);
            }

            //UtopiaLogger.Log("[KothBuildingSetsLoader] Buildings loaded and spawned from file: " + filename);
        }
        else
        {
            //UtopiaLogger.Log("[KothBuildingSetsLoader] -> Can't find JSON file in Objects folder: " + filename);
        }
    }

    void DeleteAllBuildings()
    {
        for (int i = 0; i < m_Builds.Count(); i++)
        {
            GetGame().ObjectDelete(m_Builds.Get(i));
        }
        m_Builds.Clear();
        //UtopiaLogger.Log("[KothBuildingSetsLoader] Deleted all buildings.");
    }

    private void SpawnKothBuildingObject(KothBuilding building)
    {
        if (GetGame().ConfigGetInt("CfgVehicles " + building.Name + " scope") < 1)
            return;

        Object obj = GetGame().CreateObjectEx(building.Name, building.Position, ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS);

        if (!obj)
        {
            return;
        }

        m_Builds.Insert(obj);

        obj.SetPosition(building.Position);
        obj.SetOrientation(building.Orientation);
        obj.SetOrientation(obj.GetOrientation());

        obj.SetFlags(EntityFlags.STATIC, false);
        obj.Update();
        obj.SetAffectPathgraph(true, false);
        if (obj.CanAffectPathgraph())
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 100, false, obj);
    }

    bool IsBuildingPartOfSet(Object obj) 
    {
        if (!obj) return false;

        // Iterate through the spawned buildings and check if the given object is one of them
        foreach (Object buildingObj : m_Builds) 
        {
            if (buildingObj == obj) 
            {
                return true;  // Object is part of the building set
            }
        }

        return false;  // Object is not part of the building set
    }
}

class KothBuilding
{
    string Name;
    vector Position;
    vector Orientation;

    void KothBuilding(string name = "", vector position = "0 0 0", vector orientation = "0 0 0")//skip
    {
        Name = name;
        Position = position;
        Orientation = orientation;
    }
}

// Define AttachmentData class
class LootItemAttachment
{
    string AttachName;
    int Quantity;  // -1 indicates default or not set
    float AttachmentChance;  // 0.0 to 1.0

    void LootItemAttachment(string name = "", int qty = -1, float chance = 1.0)
    {
        AttachName = name;
        Quantity = qty;
        AttachmentChance = chance;
    }
}


// Define LootItemData class
class LootItem
{
    string Name;
    int Quantity;  // -1 indicates default or not set
    float Chance;  // 0.0 to 1.0
    ref array<ref LootItem> Attachments;
    
    void LootItem(string name = "", int qty = -1, float chance = 1.0)
    {
        Name = name;
        Quantity = qty;
        Chance = chance;
        Attachments = new array<ref LootItem>();
    }
}


modded class KothPoint extends DeployableContainer_Base
{
    ref array<EntityAI> spawnedmuta = new array<EntityAI>;
    protected KothTrigger kthtrigger;
    int RandomCount;
    bool havedata;
    bool hasFinishedCapture = false;
    ref KothBuildingSetsLoader m_BuildingLoader;
    EntityAI lootContainer;
    #ifdef BASICMAP
    protected BasicMapMarker m_KothMapMarker;  // Declare marker only if BASICMAP is defined
    #endif

    private int m_markerId = -1;  // Initialize to -1 to indicate no marker

    ref KothDList kothdata = new KothDList;
    int lastWaveIndex = -1; // Track last spawned wave

    KothTrigger GetKothTrigger()
    {
        return kthtrigger;
    }

    void KothPoint()
    {
        havedata = false;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CreateTriggerDelay, 4000, false);
        //UtopiaLogger.Log("[KothPoint] KothPoint created.", "KOTH");
    }

    void ~KothPoint()
    {
        if (GetGame())
        {
            GetKothConfig().CorrectKothList(this, false);
        }
        if (GetGame() && kthtrigger)
        {
            GetGame().ObjectDelete(kthtrigger);
            kthtrigger = NULL;
        }
    }



    void SendKothData(KothDList kthdta)
    {
        kothdata = kthdta;
        havedata = true;
        hasFinishedCapture = false;  // Reset for the new event
        lastWaveIndex = -1; // Reset wave tracking when new data is sent
        //UtopiaLogger.Log("[KothPoint] Received KOTH data: " + kthdta.KothName);

        m_BuildingLoader = new KothBuildingSetsLoader(); // Initialize the building loader here
        if (kothdata.BuildingSetFile != "")
        {
            m_BuildingLoader.ReadFileAndSpawnBuildings(kothdata.BuildingSetFile);
            //UtopiaLogger.Log("[KothPoint] Building set file loaded: " + kothdata.BuildingSetFile);
        }
        else
        {
            //UtopiaLogger.Log("[KothPoint] No building set file provided.");
        }

        #ifdef BASICMAP
        CreateBasicMapMarker();  // Call the marker creation function
        #endif
        #ifdef LBmaster_Groups
        m_markerId = UtopiaMarkerManager.CreateKothMarker("KOTH Event at " + kothdata.KothName, this.GetPosition());
        //UtopiaLogger.Log("[KothPoint] LB marker created with UID: " + m_markerId, "KOTH");
        #endif
    }
    // BasicMap marker creation
    #ifdef BASICMAP
    void CreateBasicMapMarker()
    {
        string markerName = "KOTH Event at " + kothdata.KothName;
        BasicMapController basicMapController = BasicMap();
        if (basicMapController)
        {
            BasicMapMarker marker = basicMapController.CreateKOTHServerMarker(markerName, this.GetPosition());
            if (marker)
            {
                // Store the marker directly, not the UID
                m_KothMapMarker = marker;  
                //UtopiaLogger.Log("[KothPoint] BasicMap marker created successfully.");
            }
            else
            {
                //UtopiaLogger.Log("[KothPoint] Failed to create BasicMap marker.");
            }
        }
        else
        {
            //UtopiaLogger.Log("[KothPoint] BasicMapController not found.");
        }
    }
    #endif
    // LB marker creation
    #ifdef LBmaster_Groups
    void CreateLBMarker()
    {
        string markerName = "KOTH Event at " + kothdata.KothName;
        // Ensure to call the marker manager correctly, using the proper static method call or function
        LBServerMarker marker = LBStaticMarkerManager.Get.AddTempServerMarker(markerName, this.GetPosition(), "Utopia_KOTH/Datasets/Satcom.paa", ARGB(255, 255, 0, 0), false, true, true, true);
        
        if (marker)  // Ensure the marker was created successfully
        {
            m_markerId = marker.uid;  // Store the marker UID for removal later
            //UtopiaLogger.Log("[KothPoint] LB marker created with UID: " + m_markerId);
        }
        else
        {
            //UtopiaLogger.Log("[KothPoint] Failed to create LB marker");
        }
    }
    #endif
    
    void CreateTriggerDelay()
    {
        if (havedata)
        {
            vector thispos = this.GetPosition();
            NotificationSystem.SendNotificationToPlayerIdentityExtended(null, 5, GetKothConfig().cfg.NotificationHeading, string.Format(GetKothConfig().cfg.NotificationStartEvent, kothdata.KothName), "Utopia_KOTH/Datasets/Satcom.paa");
            GetKothConfig().CorrectKothList(this);

            // Create KothTrigger
            kthtrigger = KothTrigger.Cast(GetGame().CreateObjectEx("KothTrigger", thispos, ECE_PLACE_ON_SURFACE));
            if (kthtrigger)
            {
                kthtrigger.SetCollisionSphere(kothdata.TriggerRadius);
                kthtrigger.SetParentObject(this);
                kthtrigger.SetTriggerData(kothdata);
                //UtopiaLogger.Log("[KothPoint] KOTH Trigger created for event: " + kothdata.KothName, "KOTH");
            }

            // Make sure the KothPoint remains visible and synced
            SetAllowDamage(false);  // Prevents damage to the object
            SetFlags(EntityFlags.VISIBLE, false);  // Ensure visibility
            SetSynchDirty();  // Sync with clients

            // Update pathgraph if necessary
            Update();
            if (CanAffectPathgraph())
            {
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 100, false, this);
            }
        }
        // Remove else block to avoid premature deletion
    }




    float CaptureTriggerRemainingTime()
    {
        return kthtrigger.CaptureTriggerRemainingTime();  // Delegate to KothTrigger
    }

    void MonitorLootContainer()
    {
        if (lootContainer)
        {
            // Reduced interval for testing
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckLootContainerEmpty, 10000, true); 
            //UtopiaLogger.Log("[MonitorLootContainer] Started monitoring the loot container.", "KOTH");
        }
    }

    void CheckLootContainerEmpty() 
    {
        if (lootContainer && lootContainer.GetInventory() && lootContainer.GetInventory().GetCargo())
        {
            int itemCount = lootContainer.GetInventory().GetCargo().GetItemCount();
            //UtopiaLogger.Log("[CheckLootContainerEmpty] Item count in loot container: " + itemCount, "KOTH");

            if (itemCount == 0)
            {
                GetGame().ObjectDelete(lootContainer);
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(CheckLootContainerEmpty);
                //UtopiaLogger.Log("[CheckLootContainerEmpty] Loot container deleted as it was empty.", "KOTH");
            }
        }
    }





    void EndCapture()
    {
        //UtopiaLogger.Log("[KothPoint] EndCapture called.", "KOTH");
        
        // Prevent the KothPoint from being deleted or ending multiple times
        if (hasFinishedCapture)
        {
            //UtopiaLogger.Log("[KothPoint] EndCapture already finished, skipping.");
            return;
        }

        hasFinishedCapture = true;  // Set capture completion flag

        vector pos = this.GetPosition();

        // Send end notification
        NotificationSystem.SendNotificationToPlayerIdentityExtended(null, 5, GetKothConfig().cfg.NotificationHeading, string.Format(GetKothConfig().cfg.NotificationEndEvent, kothdata.KothName), "Utopia_KOTH/Datasets/Satcom.paa");

        // Delete all zombies and animals in the area
        DeleteDeadEntities(pos, kothdata.TriggerRadius);

        // Delay the deletion of KothTrigger using the configurable DeleteDelayTime
        kthtrigger.DeleteTrigger();

        // Sync KothPoint deletion with clients
        SetSynchDirty();  // Sync with clients after the capture ends

        // Remove the map marker before deleting the point
        #ifdef BASICMAP
        RemoveBMMarker();
        #endif

        #ifdef LBmaster_Groups
        UtopiaMarkerManager.RemoveLBMarker(m_markerId);  // Remove LB marker using the unified manager
        #endif

        // Delay the deletion of KothPoint to ensure loot container spawns and everything finishes
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteKothPoint, kothdata.DeleteDelayTime * 1000, false);
        //UtopiaLogger.Log("[KothPoint] KothPoint deletion scheduled after " + kothdata.DeleteDelayTime + " seconds.");

        #ifdef Utopia_Airdrop
        if (kothdata.UseAirdropFlare)
        {
            vector flarePos;
            
            // Use AirdropFlarePosition if it's set, otherwise use current KOTH position
            if (kothdata.AirdropFlarePosition != "0 0 0")  //skip
            {
                flarePos = kothdata.AirdropFlarePosition;
            }
            else
            {
                flarePos = pos;
            }

            //UtopiaLogger.Log("[KothPoint] Spawning airdrop flare at position: " + flarePos.ToString(), "KOTH");

            EntityAI flare = EntityAI.Cast(GetGame().CreateObjectEx("UtopiaAirdropFlare", flarePos, ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH));
            if (flare)
            {
                //UtopiaLogger.Log("[KothPoint] Airdrop flare spawned successfully.");
                if (flare.GetCompEM() && flare.GetCompEM().CanWork())
                {
                    flare.GetCompEM().SwitchOn();
                }
                else
                {
                    //UtopiaLogger.Log("[KothPoint] Airdrop flare component could not work.");
                }
            }
            else
            {
                //UtopiaLogger.Log("[KothPoint] Failed to spawn airdrop flare.");
            }
        }
        #endif

        if (!kothdata.UseAirdropFlare)
        {
            // Extract position and orientation for the loot container
            vector lootContainerPos = kothdata.LootContainerPosition;
            vector lootContainerOri = kothdata.LootContainerOrientation;

            // Spawn the loot container
            lootContainer = EntityAI.Cast(GetGame().CreateObjectEx(kothdata.LootContainerClass, lootContainerPos, ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH));
            if (lootContainer)
            {
                lootContainer.SetPosition(lootContainerPos);
                lootContainer.SetOrientation(lootContainerOri);
                //UtopiaLogger.Log("[KothPoint] Loot container spawned successfully.", "KOTH");

                // Optional: Update the pathgraph if the container affects pathfinding
                lootContainer.SetAffectPathgraph(true, false);
                if (lootContainer.CanAffectPathgraph())
                {
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 100, false, lootContainer);
                }

                // Sync loot container position and state with all clients
                lootContainer.SetSynchDirty();

                // Populate the loot container with items
                if (kothdata.LootItems && kothdata.LootItems.Count() > 0)
                {
                    foreach (LootItem lootItem : kothdata.LootItems)
                    {
                        SpawnItemWithAttachments(lootContainer, lootItem);
                    }
                }

                // Begin monitoring the loot container after it has been populated
                MonitorLootContainer();

                //UtopiaLogger.Log("[KothPoint] Monitoring loot container for emptiness.", "KOTH");
            }
            else
            {
                //UtopiaLogger.Log("[KothPoint] Failed to spawn loot container.");
            }
        }

    }

    void SpawnItemWithAttachments(EntityAI parent, LootItem lootItem)
    {
        if (Math.RandomFloat01() <= lootItem.Chance)
        {
            EntityAI itemEntity = null;

            // Try to create the item as an attachment
            itemEntity = parent.GetInventory().CreateAttachment(lootItem.Name);

            if (!itemEntity)
            {
                // Try to create the item in the inventory
                itemEntity = parent.GetInventory().CreateInInventory(lootItem.Name);
            }

            if (!itemEntity)
            {
                // Create the item at the parent's position in the world
                itemEntity = EntityAI.Cast(GetGame().CreateObject(lootItem.Name, parent.GetPosition()));
            }

            if (!itemEntity)
            {
                // Handle failure to create the item
                // UtopiaLogger().LogError("[KothPoint] Failed to create item: " + lootItem.Name);
                return;
            }

            ItemBase item = ItemBase.Cast(itemEntity);

            // Set quantity if specified
            if (lootItem.Quantity != -1 && item)
            {
                item = SetItemQuantity(item, lootItem.Quantity);
            }

            // Handle attachments recursively
            if (lootItem.Attachments && lootItem.Attachments.Count() > 0)
            {
                foreach (LootItem attachmentData : lootItem.Attachments)
                {
                    SpawnItemWithAttachments(itemEntity, attachmentData);
                }
            }
        }
        // else, item did not spawn due to chance
    }


    // Helper method to set quantity
    static ItemBase SetItemQuantity(ItemBase item, int quantity)
    {
        Magazine mag = Magazine.Cast(item);
        if (mag)
            mag.ServerSetAmmoCount(quantity);
        else
            item.SetQuantity(quantity);

        return item;
    }





    void DeleteDeadEntities(vector pos, float radius)
    {
        array<Object> objectsInRadius = new array<Object>;
        GetGame().GetObjectsAtPosition3D(pos, radius, objectsInRadius, NULL);
        
        foreach (Object obj : objectsInRadius)
        {
            string className = obj.GetType();
            
            // Check if it's a zombie or animal, and ensure it is dead
            if ((className.IndexOf("ZmbM_") == 0 || className.IndexOf("ZmbF_") == 0 || className.IndexOf("Animal_") == 0) && obj.IsDamageDestroyed())
            {
                GetGame().ObjectDelete(obj);
            }
        }
    }




    void DeleteKothPoint()
    {

        // Delete all buildings spawned by the building set loader
        if (m_BuildingLoader)
        {
            m_BuildingLoader.DeleteAllBuildings();  // Delete the buildings when the KothPoint is being deleted
            //UtopiaLogger.Log("[KothPoint] Buildings deleted.");
        }

        //UtopiaLogger.Log("[KothPoint] Deleting KothPoint after delay.");
        Delete();  // Call the base delete function to remove the KothPoint

        #ifdef BASICMAP
        RemoveBMMarker();
        #endif

        #ifdef LBmaster_Groups
        UtopiaMarkerManager.RemoveLBMarker(m_markerId);  // Remove LB marker using the unified manager
        #endif


    }

    #ifdef BASICMAP
    void RemoveBMMarker()
    {
        if (m_KothMapMarker)
        {
            BasicMap().KothRemoveMarker(m_KothMapMarker);  // Remove marker by passing the marker itself
        }
    }
    #endif

    #ifdef LBmaster_Groups
    void RemoveLBMarker()
    {
        if (m_markerId != -1)
        {
            //UtopiaLogger.Log("[KothPoint] Attempting to remove LB marker with UID: " + m_markerId, "KOTH");
            UtopiaMarkerManager.RemoveLBMarker(m_markerId);
            //UtopiaLogger.Log("[KothPoint] Successfully removed LB marker with UID: " + m_markerId, "KOTH");
            m_markerId = -1;  // Reset marker ID
        }
        else
        {
            //UtopiaLogger.Log("[KothPoint] No valid LB marker ID to remove.", "KOTH");
        }
    }
    #endif






    void CancelCapture()
    {
        NotificationSystem.SendNotificationToPlayerIdentityExtended(null, 5, GetKothConfig().cfg.NotificationHeading, string.Format(GetKothConfig().cfg.NotificationEndEvent, kothdata.KothName), "Utopia_KOTH/Datasets/Satcom.paa");
        kthtrigger.DeleteTrigger();
        Delete();

        #ifdef BASICMAP
        RemoveBMMarker();
        #endif

        #ifdef LBmaster_Groups
        UtopiaMarkerManager.RemoveLBMarker(m_markerId);  // Remove LB marker using the unified manager
        #endif

        //UtopiaLogger.Log("[KothPoint] KOTH event canceled: " + kothdata.KothName, "KOTH");
    }
}



class KothTrigger extends Trigger
{
    KothPoint m_ParentObj;  // Reference to the parent KothPoint
    ref KothDList trkothdata = new KothDList;
    ref Timer KTDeleteTimer;
    ref Timer CaptureTimer;
    int PlayersCount = 0;
    float CaptureTime;
    float CurrentCaptureTime;  // Time remaining for the capture
    float PlayerMult;
    bool soundPlayed = false;

    private ref Utopia_KothUI m_KothUI;  // Reference to the KOTH UI handler

    float m_CurrentCaptureProgress;  // Store the current capture progress
    int m_CurrentPlayersInZone;  // Store the number of players in the capture zone

    ref array<string> fixnotify;
    int lastWaveIndex = -1; // Track the last wave spawned based on capture progress

    float GetCurrentCaptureProgress()
    {
        return m_CurrentCaptureProgress;
    }

    int GetCurrentPlayersInZone()
    {
        return m_CurrentPlayersInZone;
    }

    void KothTrigger() 
    {
        // Initialize the UI handler for local client only
        if (GetGame().IsClient()) 
        {
            m_KothUI = new Utopia_KothUI();
            ToggleUI(false);  // Hide the UI initially
            //UtopiaLogger.Log("[KothTrigger] KOTH UI initialized for local player.");
        }
    }

    void ~KothTrigger() 
    {
        // Cleanup the UI when the trigger is destroyed
        if (GetGame().IsClient()) 
        {
            HideKothUI();
            //UtopiaLogger.Log("[KothTrigger] KOTH UI hidden during destruction.");
        }
    }

    void SetTriggerData(KothDList trkthdta) {
        trkothdata = trkthdta;
        CaptureTime = trkothdata.CaptureTime;
        CurrentCaptureTime = CaptureTime;  // Set current time to the total time at start
        PlayerMult = trkothdata.BonusLostTimeForEachPlayerOverThreshold;
        KTDeleteTimer = new Timer();
        CaptureTimer = new Timer();
        KTDeleteTimer.Run(trkthdta.TriggerTimerBeforDeleteWithoutPlayers, this, "CancelEvent", null, false);
        fixnotify = new array<string>;
        // Update the UI if on client
        if (GetGame().IsClient()) {
            UpdateKothUI();
        }

        //UtopiaLogger.Log("[KothTrigger] Trigger data set for KOTH: " + trkthdta.KothName + " with capture time: " + CaptureTime.ToString());
    }

    

    float CaptureTriggerRemainingTime()
    {
        return CurrentCaptureTime;
    }

    void SetParentObject(KothPoint obj) 
    {
        if (GetGame().IsServer()) 
        {
            m_ParentObj = obj;
            //UtopiaLogger.Log("[KothTrigger] Parent KothPoint set for KOTH: " + trkothdata.KothName);
        }
    }

    // Toggle UI visibility based on player presence
    void ToggleUI(bool show)
    {
        if (m_KothUI)
        {
            //UtopiaLogger.Log("[KothTrigger] Toggling UI visibility to: " + show.ToString());
            m_KothUI.ToggleVisibility(show);
        }
    }

    // Hide KOTH UI when the trigger is destroyed or event is canceled
    void HideKothUI() 
    {
        if (m_KothUI) 
        {
            ToggleUI(false);
        }
    }

    // Update the UI with capture progress and player count
    void UpdateKothUI()
    {
        if (GetGame().IsClient())
        {
            // Call the global event to update UI
            KothEventManager.OnUpdateKothUI.Invoke(m_CurrentCaptureProgress, m_CurrentPlayersInZone);
            //UtopiaLogger.Log("[KothTrigger] UI updated via event: CaptureProgress = " + m_CurrentCaptureProgress + ", PlayersInZone = " + m_CurrentPlayersInZone);
        }
    }

    void SendUIUpdateToClient(PlayerIdentity identity, float captureProgress, int playersInZone)
    {
        if (GetGame().IsServer())
        {
            //UtopiaLogger.Log("[SendUIUpdateToClient] Preparing to send RPC to client with identity: " + identity.GetName() + ", Capture Progress: " + captureProgress + ", Players in Zone: " + playersInZone);
            auto params = new Param2<float, int>(captureProgress, playersInZone);
            GetRPCManager().SendRPC("Utopia_Koth", "HandleKothUIUpdateRPC", params, true, identity);
            //UtopiaLogger.Log("[SendUIUpdateToClient] Sent UI update to client.");
        }
    }



    bool IsLivePlayer(Object obj) {
        PlayerBase player = PlayerBase.Cast(obj);
        return player && player.GetIdentity() && player.IsAlive();
    }

    // Get the count of live players in the capture zone
    int GetLivePlayerCount() {
        int count = 0;
        foreach (TriggerInsider insider : m_insiders) {
            Object obj = insider.GetObject();
            if (IsLivePlayer(obj)) {
                count++;
            }
        }
        return count;
    }

    // Clean up dead players from the insider list
    void CleanUpDeadPlayers() {
        for (int i = m_insiders.Count() - 1; i >= 0; i--) {
            Object obj = m_insiders[i].GetObject();
            PlayerBase player = PlayerBase.Cast(obj);
            if (player && !player.IsAlive()) {
                // Remove the dead player from insiders
                RemoveInsider(m_insiders[i], i);
                //UtopiaLogger.Log("[KothTrigger] Removed dead player from insiders: " + player.GetIdentity().GetName());
            }
        }
    }





    override void OnEnter(Object obj)
    {
        super.OnEnter(obj);

        if (obj.IsMan() && GetGame().IsServer())
        {
            PlayerBase player = PlayerBase.Cast(obj);
            if (player && player.GetIdentity())
            {
                //UtopiaLogger.Log("[KothTrigger] Player entered KOTH area: " + player.GetIdentity().GetName(), "KOTH");
                NotificationSystem.SendNotificationToPlayerExtended(player, 3.0, GetKothConfig().cfg.NotificationHeading, GetKothConfig().cfg.NotificationEnter, "Utopia_KOTH/Datasets/Satcom.paa");

                // Send an RPC to the client to show the UI when the player enters the KOTH area
                GetRPCManager().SendRPC("Utopia_Koth", "OnEnterKothZone", null, true, player.GetIdentity());

                UpdatePlayersInZone();  // Update the number of players in the capture zone
            }
        }
    }

    override void OnLeave(Object obj)
    {
        super.OnLeave(obj);

        if (obj.IsMan() && GetGame().IsServer())
        {
            PlayerBase player = PlayerBase.Cast(obj);
            if (player && player.GetIdentity())
            {
                //UtopiaLogger.Log("[KothTrigger] Player left KOTH area: " + player.GetIdentity().GetName(), "KOTH");
                NotificationSystem.SendNotificationToPlayerExtended(player, 3.0, GetKothConfig().cfg.NotificationHeading, GetKothConfig().cfg.NotificationExit, "Utopia_KOTH/Datasets/Satcom.paa");

                // Send an RPC to the client to hide the UI when the player leaves the KOTH area
                GetRPCManager().SendRPC("Utopia_Koth", "OnExitKothZone", null, true, player.GetIdentity());

                UpdatePlayersInZone();  // Update the number of players in the capture zone
            }
        }
    }



    override protected void AddInsider(Object obj) 
    {
        super.AddInsider(obj);

        // Ensure only live players are counted
        if (!IsLivePlayer(obj)) 
        {
            //UtopiaLogger.Log("[KothTrigger] Excluded non-player or dead entity from capture area: " + obj.GetType());
            return;  // Skip counting this object
        }

        if (GetGame().IsServer()) 
        {
            //UtopiaLogger.Log("[KothTrigger] Live player entered KOTH area, current players: " + GetLivePlayerCount().ToString());

            // Update UI when a player enters
            UpdatePlayersInZone();

            if (GetLivePlayerCount() >= trkothdata.MinPlayersToCapture) 
            {
                if (KTDeleteTimer.IsRunning()) 
                {
                    KTDeleteTimer.Stop();
                }
                if (!CaptureTimer.IsRunning()) 
                {
                    CaptureTimer.Run(1, this, "CaptureCheck", null, true);
                    //UtopiaLogger.Log("[KothTrigger] Capture started.", "KOTH");
                    PlaySoundForNearbyPlayers();
                }
            } 
            else 
            {
                if (!KTDeleteTimer.IsRunning()) 
                {
                    KTDeleteTimer.Run(trkothdata.TriggerTimerBeforDeleteWithoutPlayers, this, "CancelEvent", null, false);
                }
                if (CaptureTimer.IsRunning()) 
                {
                    CaptureTimer.Stop();
                    //UtopiaLogger.Log("[KothTrigger] Capture stopped, not enough players.", "KOTH");
                }
            }
        }
    }

    override protected void RemoveInsider(TriggerInsider insider, int index = -1) 
    {
        super.RemoveInsider(insider, index);

        Object obj = insider.GetObject();

        if (!IsLivePlayer(obj)) 
        {
            //UtopiaLogger.Log("[KothTrigger] Removed non-player or dead entity from capture area: " + obj.GetType());
            return;
        }

        if (GetGame().IsServer()) 
        {
            //UtopiaLogger.Log("[KothTrigger] Live player left KOTH area, current players: " + GetLivePlayerCount().ToString());

            // Update UI when a player leaves
            UpdatePlayersInZone();

            if (GetLivePlayerCount() >= trkothdata.MinPlayersToCapture) 
            {
                if (KTDeleteTimer.IsRunning()) 
                {
                    KTDeleteTimer.Stop();
                }
                if (!CaptureTimer.IsRunning()) 
                {
                    CaptureTimer.Run(1, this, "CaptureCheck", null, true);
                    //UtopiaLogger.Log("[KothTrigger] Capture resumed.");
                }
            } 
            else 
            {
                if (!KTDeleteTimer.IsRunning()) 
                {
                    KTDeleteTimer.Run(trkothdata.TriggerTimerBeforDeleteWithoutPlayers, this, "CancelEvent", null, false);
                }
                if (CaptureTimer.IsRunning()) 
                {
                    CaptureTimer.Stop();
                    //UtopiaLogger.Log("[KothTrigger] Capture stopped, not enough players.");
                }
            }
        }
    }

    void PlaySoundForNearbyPlayers()
    {
        // Define the maximum hearing distance
        float maxDistance = 1000.0;
        vector eventPosition = m_ParentObj.GetPosition();

        // Find all players in the game
        ref array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);

        // Iterate over all players and play the sound for those within the distance
        foreach (Man player : players)
        {
            if (player && player.IsAlive() && player.IsInherited(PlayerBase))
            {
                PlayerBase pb = PlayerBase.Cast(player);
                if (pb && pb.GetIdentity())
                {
                    // Check if the player is within the max distance
                    float distanceToEvent = vector.Distance(pb.GetPosition(), eventPosition);
                    if (distanceToEvent <= maxDistance)
                    {
                        // Use PlaySoundSetAtPos to play the sound at the event location
                        EffectSound sound = SEffectManager.PlaySound("Utopia_KOTH_SoundSet", eventPosition);
                        sound.SetSoundAutodestroy( true );
                        //UtopiaLogger.Log("[KothTrigger] Capture progress sound played for player: " + pb.GetIdentity().GetName() + " at distance: " + distanceToEvent.ToString());
                    }
                    else
                    {
                        //UtopiaLogger.Log("[KothTrigger] Player " + pb.GetIdentity().GetName() + " is too far to hear the sound (" + distanceToEvent.ToString() + " meters).");
                    }
                }
            }
        }
    }





    void DeleteTrigger()
    {
        if (GetGame().IsServer())
        {
            // Delay deletion based on the configurable DeleteDelayTime
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteTriggerWithDelay, trkothdata.DeleteDelayTime * 1000, false);
        }
    }

    // New function to actually delete the KothTrigger after the delay
    void DeleteTriggerWithDelay()
    {
        if (GetGame().IsServer())
        {
            GetGame().ObjectDelete(this);
            //UtopiaLogger.Log("[KOTH] KothTrigger deleted after delay.");
        }
    }


    void UpdatePlayersInZone()
    {
        m_CurrentPlayersInZone = GetLivePlayerCount();
        UpdateKothUI();  // Update the UI locally

        //UtopiaLogger.Log("[UpdatePlayersInZone] Current players in zone: " + m_CurrentPlayersInZone.ToString());

        // Get the player identity and send the UI update to all clients in the zone
        foreach (TriggerInsider insider : m_insiders)
        {
            PlayerBase player = PlayerBase.Cast(insider.GetObject());
            if (player && player.GetIdentity() && player.IsAlive())
            {
                //UtopiaLogger.Log("[UpdatePlayersInZone] Sending UI update to player: " + player.GetIdentity().GetName());
                SendUIUpdateToClient(player.GetIdentity(), m_CurrentCaptureProgress, m_CurrentPlayersInZone);
            }
        }
    }



    void CaptureCheck()
    {
        // Clean up dead players before calculating capture progress
        CleanUpDeadPlayers();

        if (CurrentCaptureTime > 0)
        {
            int livePlayerCount = GetLivePlayerCount();
            float TimerMult = (livePlayerCount - trkothdata.MinPlayersToCapture) * PlayerMult;
            CurrentCaptureTime -= (1 + TimerMult);

            m_CurrentCaptureProgress = (trkothdata.CaptureTime - CurrentCaptureTime) / trkothdata.CaptureTime;  // Calculate capture percentage
            //UtopiaLogger.Log("[KothTrigger] Capture progress: " + (m_CurrentCaptureProgress * 100).ToString() + "%");

            // Broadcast RPC to all clients with the updated capture progress and players in the zone
            foreach (TriggerInsider insider : m_insiders)  // Use TriggerInsider instead of Object for type safety
            {
                PlayerBase player = PlayerBase.Cast(insider.GetObject());  // Retrieve player from insider object
                if (player && player.GetIdentity() && player.IsAlive())
                {
                    //UtopiaLogger.Log("[CaptureCheck] Sending UI update to player: " + player.GetIdentity().GetName());
                    
                    Param2<float, int> params = new Param2<float, int>(m_CurrentCaptureProgress, m_CurrentPlayersInZone);
                    GetRPCManager().SendRPC("Utopia_Koth", "HandleKothUIUpdateRPC", params, true, player.GetIdentity());
                }
            }

            // Check if a wave should be spawned based on capture progress
            float progressPercentage = m_CurrentCaptureProgress * 100;
            SpawnEnemyWave(progressPercentage);
        }
        else
        {
            //UtopiaLogger.Log("[KothTrigger] Capture time reached zero, calling CaptureFinish.");
            CaptureFinish();
        }
    }









    void SpawnEnemyWave(float progressPercentage)
    {
        for (int i = lastWaveIndex + 1; i < trkothdata.CaptureProgressTriggers.Count(); i++)
        {
            if (progressPercentage >= trkothdata.CaptureProgressTriggers[i])
            {
                // Spawn the enemy wave
                WaveData wave = trkothdata.EnemyWaves[i];
                foreach (string entityClass : wave.EntitiesToSpawn)
                {
                    vector spawnPosition = m_ParentObj.GetPosition() + Vector(Math.RandomFloat(-10, 10), 0, Math.RandomFloat(-10, 10));
                    GetGame().CreateObject(entityClass, spawnPosition, false, true);
                    //UtopiaLogger.Log("[KothTrigger] Spawned enemy wave entity: " + entityClass + " at position: " + spawnPosition.ToString());
                }
                lastWaveIndex = i;
            }
        }
    }

    void CaptureFinish() 
    {
        if (CaptureTimer.IsRunning()) 
        {
            CaptureTimer.Stop();  // Stop the capture timer when the event finishes
        }

        //UtopiaLogger.Log("[KothTrigger] Capture finished, notifying KothPoint.");
        if (GetGame().IsClient()) 
        {
            ToggleUI(false);  // Hide UI when the capture finishes
        }

        if (!m_ParentObj.hasFinishedCapture) 
        {
            m_ParentObj.EndCapture();  // Notify the KothPoint to handle the event completion
        }
    }





    void CancelEvent()
    {
        //UtopiaLogger.Log("[KothTrigger] KOTH event canceled.");
        m_ParentObj.CancelCapture();
    }
}


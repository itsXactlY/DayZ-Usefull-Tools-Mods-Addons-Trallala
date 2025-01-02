modded class IngameHud
{
    private ref Utopia_KothUI m_UtopiaKothUI;
    private bool m_IsInKothZone = false;  // Track if the player is in the KOTH zone
    private bool m_HudVisible = true;     // Assume HUD is visible by default
    private const int KOTH_UI_UPDATE_INTERVAL = 1000; // Update every 1 second

    override void Init(Widget hud_panel_widget)
    {
        super.Init(hud_panel_widget);

        // Initialize the KOTH UI element
        m_UtopiaKothUI = new Utopia_KothUI();
        //UtopiaLogger.Log("[IngameHud] Utopia_KothUI initialized.", "KOTH");

        // Register RPC handlers
        GetRPCManager().AddRPC("Utopia_Koth", "HandleKothUIUpdateRPC", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("Utopia_Koth", "OnEnterKothZone", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("Utopia_Koth", "OnExitKothZone", this, SingleplayerExecutionType.Client);

        // Start the tick scheduler
        StartKothUITick();
    }

    void StartKothUITick()
    {
        // Schedule the UpdateKothUI method to be called every second
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateKothUI, KOTH_UI_UPDATE_INTERVAL, true);
    }

    void StopKothUITick()
    {
        // Remove the scheduled call to prevent memory leaks
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(UpdateKothUI);
    }

    void UpdateKothUI()
    {
        if (m_IsInKothZone && m_HudVisible)
        {
            // Request the latest KOTH data from the server
            GetRPCManager().SendRPC("Utopia_Koth", "RequestKothUIUpdate", null, true);
        }
    }

    override void ShowHud(bool show)
    {
        super.ShowHud(show);

        m_HudVisible = show;
        UpdateKothUIVisibility();
    }

    override void ShowHudUI(bool show)
    {
        super.ShowHudUI(show);

        m_HudVisible = show;
        UpdateKothUIVisibility();
    }

    void UpdateKothUIVisibility()
    {
        if (m_UtopiaKothUI)
        {
            bool shouldShow = m_IsInKothZone && m_HudVisible;
            m_UtopiaKothUI.ToggleVisibility(shouldShow);
            //UtopiaLogger.Log("[IngameHud] KOTH UI visibility updated to: " + shouldShow.ToString(), "KOTH");
        }
    }

    // RPC handler to receive UI updates from the server
    void HandleKothUIUpdateRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Client)
        {
            Param2<float, int> data;
            if (!ctx.Read(data))
            {
                //UtopiaLogger.Log("[HandleKothUIUpdateRPC] Failed to read params from the context.", "KOTH");
                return;
            }

            float captureProgress = data.param1;
            int playersInZone = data.param2;

            //UtopiaLogger.Log("[HandleKothUIUpdateRPC] Received data from server. Progress: " + captureProgress + ", Players in Zone: " + playersInZone, "KOTH");

            // Update the KOTH UI if in the zone
            if (m_UtopiaKothUI && m_IsInKothZone)
            {
                m_UtopiaKothUI.UpdateKOTHCaptureProgress(captureProgress, playersInZone);
            }
        }
    }

    // RPC handler to show the KOTH UI when entering the zone
    void OnEnterKothZone(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Client)
        {
            m_IsInKothZone = true;
            UpdateKothUIVisibility();
            //UtopiaLogger.Log("[IngameHud] Entered KOTH zone. Showing UI.", "KOTH");
        }
    }

    // RPC handler to hide the KOTH UI when exiting the zone
    void OnExitKothZone(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Client)
        {
            m_IsInKothZone = false;
            UpdateKothUIVisibility();
            //UtopiaLogger.Log("[IngameHud] Exited KOTH zone. Hiding UI.", "KOTH");
        }
    }

    void ~IngameHud()
    {
        // Stop the tick scheduler when the HUD is destroyed
        StopKothUITick();

        if (m_UtopiaKothUI)
        {
            m_UtopiaKothUI.ToggleVisibility(false);
            m_UtopiaKothUI = null;
        }
    }
}

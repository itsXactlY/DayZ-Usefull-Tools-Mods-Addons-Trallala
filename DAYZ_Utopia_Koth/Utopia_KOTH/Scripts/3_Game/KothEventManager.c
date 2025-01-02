class KothEventManager
{
    static ref KothEventManager s_Instance;  // Singleton instance
    static ref ScriptInvoker OnUpdateKothUI = new ScriptInvoker();

    // Constructor
    void KothEventManager()
    {
        s_Instance = this;  // Set the singleton instance when the object is created
    }

    // Static method to initialize the manager
    static void Init()
    {
        if (!s_Instance)
        {
            s_Instance = new KothEventManager();
        }
    }

    // Example method to update the KOTH UI (customize as needed)
    void UpdateKothUI(float captureProgress, int playersInZone)
    {
        //UtopiaLogger.Log("[KothEventManager] UpdateKothUI invoked with captureProgress: " + captureProgress + ", playersInZone: " + playersInZone);
        OnUpdateKothUI.Invoke(captureProgress, playersInZone);  // Trigger the invoker with new data
    }


    static void NotifyEnterKothZone(PlayerIdentity identity)
    {
        GetRPCManager().SendRPC("Utopia_Koth", "OnEnterKothZone", null, true, identity);
    }

    // Method to notify UI update for exiting the KOTH zone
    static void NotifyExitKothZone(PlayerIdentity identity)
    {
        GetRPCManager().SendRPC("Utopia_Koth", "OnExitKothZone", null, true, identity);
    }

    // Optional: Additional methods to handle events can be added here
}

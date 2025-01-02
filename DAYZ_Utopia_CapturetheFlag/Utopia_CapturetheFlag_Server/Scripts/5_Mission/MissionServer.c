modded class MissionServer
{
    private ref CTFEventManager m_CTFEventManager;

    // Constructor
    void MissionServer()
    {
        // Log that the MissionServer has been constructed and the mod is initializing
        UtopiaLogger.Log("MissionServer modded class initialized for CTF Mod", "CTF");

        // Initialize the CTF configuration
        InitializeCTFMod();
    }

    // Initialize the Capture the Flag mod and its components
    void InitializeCTFMod()
    {
        // Log the initialization process
        UtopiaLogger.Log("Initializing Capture the Flag Mod...", "CTF");

        // Load or create the global CTF configuration
        g_CTFConfig = new CTFConfig();
        UtopiaLogger.Log("CTF Configuration loaded or created successfully.", "CTF");

        // Initialize the CTF Event Manager
        m_CTFEventManager = new CTFEventManager();
        UtopiaLogger.Log("CTF Event Manager initialized successfully.", "CTF");

        // Optionally start the event immediately or add additional setup here
        m_CTFEventManager.StartEvent();
        UtopiaLogger.Log("CTF Event started successfully.", "CTF");
    }

    override void OnInit()
    {
        super.OnInit();

        // Additional initialization steps if needed
        UtopiaLogger.Log("MissionServer OnInit called.", "CTF");
    }

    override void OnMissionStart()
    {
        super.OnMissionStart();
        UtopiaLogger.Log("MissionServer OnMissionStart called.", "CTF");

        // Start the CTF event when the mission starts
        if (m_CTFEventManager) {
            m_CTFEventManager.StartEvent();
            UtopiaLogger.Log("CTF Event started from OnMissionStart.", "CTF");
        }
    }

    override void OnMissionFinish()
    {
        super.OnMissionFinish();

        // Log that the mission has finished and perform any cleanup
        UtopiaLogger.Log("MissionServer OnMissionFinish called. Cleaning up CTF resources.", "CTF");

        // Clean up resources
        if (m_CTFEventManager) {
            m_CTFEventManager.Cleanup();
            m_CTFEventManager = null;
            UtopiaLogger.Log("CTF Event Manager cleaned up successfully.", "CTF");
        }
    }
}

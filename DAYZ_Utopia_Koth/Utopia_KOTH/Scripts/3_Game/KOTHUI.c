class Utopia_KothUI : Managed
{
    Widget m_RootWidget;
    protected ProgressBarWidget m_ProgressBar;
    protected TextWidget m_PlayerCount;

    private string m_LastPlayerCountText = "";  // Track last known player count text

    void Utopia_KothUI()
    {
        // Load the layout file and log the initialization process.
        m_RootWidget = GetGame().GetWorkspace().CreateWidgets("Utopia_KOTH/Datasets/kothui.layout");
        if (!m_RootWidget)
        {
            //UtopiaLogger.Log("[Utopia_KothUI] Failed to create root widget from layout file.", "KOTH");
            return;
        }

        //UtopiaLogger.Log("[Utopia_KothUI] Layout successfully created. Retrieving widget references.", "KOTH");

        // Retrieve references to the widgets using the name attribute.
        m_ProgressBar = ProgressBarWidget.Cast(m_RootWidget.FindAnyWidget("ProgressBar"));
        if (!m_ProgressBar)
        {
            //UtopiaLogger.Log("[Utopia_KothUI] Failed to find ProgressBar widget.", "KOTH");
            return;
        }
        //UtopiaLogger.Log("[Utopia_KothUI] ProgressBar widget successfully initialized.", "KOTH");

        m_PlayerCount = TextWidget.Cast(m_RootWidget.FindAnyWidget("PlayerCount"));
        if (!m_PlayerCount)
        {
            //UtopiaLogger.Log("[Utopia_KothUI] Failed to find PlayerCount widget.", "KOTH");
            return;
        }
        //UtopiaLogger.Log("[Utopia_KothUI] PlayerCount widget successfully initialized.", "KOTH");

        // Ensure widgets are visible.
        m_ProgressBar.Show(true);
        m_PlayerCount.Show(false);

        // Set initial visibility of the root widget to false.
        ToggleVisibility(false);
        //UtopiaLogger.Log("[Utopia_KothUI] Initialized successfully.", "KOTH");
    }

    void ToggleVisibility(bool show)
    {
        if (m_RootWidget)
        {
            m_RootWidget.Show(show);
            //UtopiaLogger.Log("[Utopia_KothUI] Root widget visibility set to: " + show.ToString(), "KOTH");
        }
    }

    void UpdateKOTHCaptureProgress(float captureProgress, int playersInZone)
    {
        //UtopiaLogger.Log("[Utopia_KothUI] Updating UI with capture progress: " + captureProgress + " and players in zone: " + playersInZone, "KOTH");

        if (m_ProgressBar && m_PlayerCount)
        {
            m_ProgressBar.SetCurrent(captureProgress * 100); // Assuming progress bar expects a value between 0 and 100
            m_PlayerCount.SetText("Players: " + playersInZone.ToString());

            m_ProgressBar.Update();
            m_PlayerCount.Update();
        }
        else
        {
            //UtopiaLogger.Log("[Utopia_KothUI] Widget references are null.", "KOTH");
        }
    }



    void ~Utopia_KothUI()
    {
        if (m_RootWidget)
        {
            m_RootWidget.Show(false);
            m_RootWidget.Unlink();
        }
    }
}

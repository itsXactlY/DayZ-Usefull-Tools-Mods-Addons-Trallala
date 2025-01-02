class CTFTickScheduler {
    private ref Timer m_TickTimer;             // Timer to handle scheduling ticks
    private CTFEventManager m_EventManager;    // Reference to the CTF event manager
    private float m_TickInterval;              // Interval (in seconds) between each tick

    void CTFTickScheduler(CTFEventManager eventManager, float tickInterval = 1.0) {
        m_EventManager = eventManager;
        m_TickInterval = tickInterval;
        m_TickTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
    }

    // Start the tick scheduler with the current interval
    void Start() {
        if (!m_TickTimer.IsRunning()) {
            Print("[CTFTickScheduler] Starting tick scheduler with interval: " + m_TickInterval + " seconds.");
            m_TickTimer.Run(m_TickInterval, this, "OnTick", null, true); // Repeat indefinitely
        }
    }

    // Stop the tick scheduler
    void Stop() {
        if (m_TickTimer.IsRunning()) {
            m_TickTimer.Stop();
            Print("[CTFTickScheduler] Stopping tick scheduler.");
        }
    }

    // Change the tick interval dynamically
    void SetInterval(float newInterval) {
        // Only change interval if it's a valid value and different from the current one
        if (newInterval > 0 && newInterval != m_TickInterval) {
            Print("[CTFTickScheduler] Changing tick interval from " + m_TickInterval + " seconds to " + newInterval + " seconds.");
            m_TickInterval = newInterval;
            
            // Restart the timer with the new interval if it's already running
            if (m_TickTimer.IsRunning()) {
                Stop();
                Start();
            }
        }
    }

    // Called on every tick
    void OnTick() {
        // Ensure we have a valid event manager reference
        if (m_EventManager) {
            // Force update the map markers
            m_EventManager.UpdateFlagMarker();
            Print("[CTFTickScheduler] Forced update of map markers at interval: " + m_TickInterval + " seconds.");
        }
    }
}

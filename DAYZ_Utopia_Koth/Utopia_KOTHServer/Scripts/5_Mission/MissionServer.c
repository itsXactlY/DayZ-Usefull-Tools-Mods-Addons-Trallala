modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();

        GetRPCManager().AddRPC("Utopia_Koth", "RequestKothUIUpdate", this, SingleplayerExecutionType.Server);

        GetRPCManager().AddRPC("Utopia_Koth", "HandleKothUIUpdateRPC", this, SingleplayerExecutionType.Client);

        KothEventManager.Init();

        //UtopiaLogger.Log("[MissionServer] Server initialized and RPCs registered.", "KOTH");
    }

    void RegisterKothRPCs()
    {
        if (GetGame().IsServer())
        {
            GetRPCManager().AddRPC("Utopia_Koth", "OnEnterKothZone", this, SingleplayerExecutionType.Both);
            GetRPCManager().AddRPC("Utopia_Koth", "OnExitKothZone", this, SingleplayerExecutionType.Both);
        }
    }

    void HandleKothUIUpdateRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Client)
        {
            //UtopiaLogger.Log("[HandleKothUIUpdateRPC] RPC received on client.", "KOTH");

            Param2<float, int> params;
            if (!ctx.Read(params))
            {
                //UtopiaLogger.Log("[HandleKothUIUpdateRPC] Failed to read RPC parameters.", "KOTH");
                return;
            }

            float captureProgress = params.param1;
            int playersInZone = params.param2;

            //UtopiaLogger.Log("[HandleKothUIUpdateRPC] Received captureProgress: " + captureProgress + ", playersInZone: " + playersInZone, "KOTH");

            if (!KothEventManager.s_Instance)
            {
                //UtopiaLogger.Log("[HandleKothUIUpdateRPC] KothEventManager instance is not initialized. Initializing now.", "KOTH");
                KothEventManager.Init();
            }

            if (KothEventManager.s_Instance)
            {
                KothEventManager.s_Instance.UpdateKothUI(captureProgress, playersInZone);
                //UtopiaLogger.Log("[HandleKothUIUpdateRPC] Successfully triggered UI update with captureProgress: " + captureProgress + " and playersInZone: " + playersInZone, "KOTH");
            }
            else
            {
                //UtopiaLogger.Log("[HandleKothUIUpdateRPC] Failed to trigger UI update. KothEventManager instance is still null.", "KOTH");
            }
        }
    }

    void RequestKothUIUpdate(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Server && sender)
        {
            KothTrigger activeTrigger = GetActiveKothTrigger();
            if (activeTrigger)
            {
                float captureProgress = activeTrigger.GetCurrentCaptureProgress();
                int playersInZone = activeTrigger.GetCurrentPlayersInZone();

                Param2<float, int> data = new Param2<float, int>(captureProgress, playersInZone);
                GetRPCManager().SendRPC("Utopia_Koth", "HandleKothUIUpdateRPC", data, true, sender);

                //UtopiaLogger.Log("[RequestKothUIUpdate] Sent KOTH data to client: " + sender.GetName(), "KOTH");
            }
            else
            {
                //UtopiaLogger.Log("[RequestKothUIUpdate] No active KOTH trigger found.", "KOTH");
            }
        }
    }

    KothTrigger GetActiveKothTrigger()
    {
        if (g_KothConfig && g_KothConfig.kothlist.Count() > 0)
        {
            KothPoint kothPoint = g_KothConfig.kothlist[0];
            if (kothPoint)
            {
                KothTrigger activeTrigger = kothPoint.GetKothTrigger();
                if (activeTrigger)
                {
                    return activeTrigger;
                }
            }
        }
        return null;
    }


}

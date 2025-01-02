modded class ModItemRegisterCallbacks
{
	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterHeavy(pType, pBehavior);
        pType.AddItemInHandsProfileIK("Utopia_CaptureFlag", "dz/anims/workspaces/player/player_main/weapons/player_main_2h_extinguisher.asi", pBehavior, 		"dz/anims/anm/player/ik/two_handed/alice_g.anm");    
    }
};
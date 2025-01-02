class KothPoint: DeployableContainer_Base
{
	protected bool m_IsKothOpen = false;
	void KothPoint()
	{
		RegisterNetSyncVariableBool( "m_IsKothOpen" );
	}
	
	override bool CanDisplayCargo()
	{		
		return m_IsKothOpen;
	}
	
	    override bool CanPutInCargo( EntityAI parent )
    {
        return false;
    }
    
    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }
};

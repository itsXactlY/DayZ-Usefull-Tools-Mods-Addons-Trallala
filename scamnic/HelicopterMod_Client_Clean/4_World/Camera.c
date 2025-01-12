modded class DayZPlayerCamera3rdPersonVehicle
{
	
	override void 		OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		if ( m_pPlayer )
		{
			Transport vehicle;
			if ( Class.CastTo( vehicle, m_pPlayer.GetParent() ) )
			{
				HeliTest_SIB heli = HeliTest_SIB.Cast(vehicle);
				if(heli)
				{
					m_fDistance = heli.GetCameraHeight();
				}
			}
		}
		super.OnUpdate(pDt, pOutResult);
	}	
}




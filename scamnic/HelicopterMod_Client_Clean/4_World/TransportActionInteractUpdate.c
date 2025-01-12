

//ActionStartEngine
modded class ActionStartEngine
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if( vehCommand )
		{
			Transport trans = vehCommand.GetTransport();
			
			if ( trans )
			{
				CarScript car;
				if ( Class.CastTo(car, trans) && !car.EngineIsOn() )
				{
					if (car && car.m_typeVehicle > 0)return false;	
					if ( car.GetHealthLevel("Engine") >= GameConstants.STATE_RUINED )
					return false;
					
					if ( car.CrewMemberIndex( player ) == DayZPlayerConstants.VEHICLESEAT_DRIVER )
					return true;
				}
			}
		}

		return false;
	}
}
//ActionStopEngine
modded class ActionStopEngine
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		HumanCommandVehicle vehCmd = player.GetCommand_Vehicle();
		CarScript car;
		
		if ( vehCmd && vehCmd.GetVehicleSeat() == DayZPlayerConstants.VEHICLESEAT_DRIVER )
		{
			Transport trans = vehCmd.GetTransport();
			if ( trans )
			{
				if ( Class.CastTo(car, trans) && car.EngineIsOn() )
				{
					if (car && car.m_typeVehicle > 0)return false;	
					if ( car.GetSpeedometer() <= 8 )
					return true;
				}
			}
		}
		return false;
	}
}

//ActionSwitchSeats
modded class ActionSwitchSeats
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		
		Transport trans = null;
		int nextSeatIdx = -1;
		
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();

		if ( !vehCommand  )
		return false;
		
		int componentIndex = target.GetComponentIndex();

		if ( !target )
		return false;

		if ( !Class.CastTo(trans, target.GetObject()) )
		return false;
	
			CarScript car;
				if ( Class.CastTo(car, trans) )
				{
					if (car && car.m_typeVehicle > 0)return false;	
				}

		nextSeatIdx = trans.CrewPositionIndex( componentIndex );

		if ( nextSeatIdx < 0 )
		return false;

		Human crew = trans.CrewMember( nextSeatIdx );
		if ( crew )
		return false;

		if ( !trans.CanReachSeatFromSeat( trans.CrewMemberIndex( player ), nextSeatIdx ) )
		return false;

		return true;
	}
}

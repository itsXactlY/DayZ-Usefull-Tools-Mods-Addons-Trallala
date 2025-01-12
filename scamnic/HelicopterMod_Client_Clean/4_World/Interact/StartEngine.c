class ActionStartEngineHeliCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(1.0);
	}
};

class ActionHeliStartEngineSIB: ActionContinuousBase
{
	void ActionHeliStartEngineSIB()
	{
		m_CallbackClass = ActionStartEngineHeliCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_STARTENGINE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_SpecialtyWeight = 0;
		m_LockTargetOnUse = false;
	}
	
	void StartEngineSIB(HeliTest_SIB heli){};
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNone;
		m_ConditionItem = new CCINone;

	}
	
	override string GetText()
	{
		return "Start Engine";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		HeliTest_SIB trans = null;
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();
		if ( !vehCommand  )return false;	
		
		if ( !target )return false;
		
		if ( !Class.CastTo(trans, target.GetObject()) )return false;
		Human crew = trans.CrewMember(0);
		if ( !crew )
		{
			crew = trans.CrewMember(1);
			if ( !crew )return false;
		}
		
		int crewIdx = trans.CrewMemberIndex( player );
		if(crewIdx>1)return false;
		
		if(!trans.m_enable)return false;
		if (trans.m_typeVehicle == 0)return false;
		float anim_engine = trans.GetAnimationPhase("hide_start");
		if(anim_engine >= 0.5)return false;

		
		array<string> selections = new array<string>;
		target.GetObject().GetActionComponentNameList(target.GetComponentIndex(), selections);

		
		for (int s = 0; s < selections.Count(); s++)
		{
			if ( selections[s] == "avtopilot")
				return false;
		}
		
		return true;
	}

	override bool CanBeUsedInVehicle()
	{
		return true;
	}
};
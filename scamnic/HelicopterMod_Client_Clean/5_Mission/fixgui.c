
modded class ActionTargetsCursor
{
	protected override void GetActions()
	{
		m_Interact = null;
		m_ContinuousInteract = null;
		m_Single = null;
		m_Continuous = null;

		if(!m_AM) return;
		if(!m_Target) return;
		if(m_Player.IsSprinting()) return;
		
		HeliTest_SIB trans = null;
		HumanCommandVehicle vehCommand = m_Player.GetCommand_Vehicle();
		if (vehCommand )
		{
		if (Class.CastTo(trans,vehCommand.GetTransport()) )
		{
			
		}else{
		if(m_Player.IsInVehicle()) return; 
		}
		}else{	
		if(m_Player.IsInVehicle()) return;
		}
		
			
		array<ActionBase> possible_interact_actions = m_AM.GetPossibleActions(InteractActionInput);
		array<ActionBase> possible_continuous_interact_actions = m_AM.GetPossibleActions(ContinuousInteractActionInput);
		int possible_interact_actions_index = m_AM.GetPossibleActionIndex(InteractActionInput);
		int possible_continuous_interact_actions_index = m_AM.GetPossibleActionIndex(ContinuousInteractActionInput);
		
		m_InteractActionsNum = possible_interact_actions.Count();
		if( m_InteractActionsNum > 0 )
		{
			m_Interact = possible_interact_actions[possible_interact_actions_index];
		}
		
		m_ContinuousInteractActionsNum = possible_continuous_interact_actions.Count();
		if( m_ContinuousInteractActionsNum > 0 )
		{
			m_ContinuousInteract = possible_continuous_interact_actions[possible_continuous_interact_actions_index];
		}
		
		
		m_Single = m_AM.GetPossibleAction(DefaultActionInput);
		m_Continuous = m_AM.GetPossibleAction(ContinuousDefaultActionInput);
	}
	
		protected override void GetTarget()
	{
		if(!m_AM) return;

		m_Target = m_AM.FindActionTarget();
		m_Hidden = false;

		if (m_Target && m_Target.GetObject() && m_Target.GetObject().IsItemBase())
		{
			ItemBase item = ItemBase.Cast(m_Target.GetObject());
			if( !item.IsTakeable())
			{
				m_Hidden = true;
			}
		}
	}
};

modded class ItemActionsWidget
{
	    protected override void GetActions()
	{
		m_Interact = null;
		m_ContinuousInteract = null;
		m_Single = null;
		m_Continuous = null;

		if(!m_AM) return;
		//if(!m_EntityInHands) return false;
		if(m_Player.IsSprinting()) return;
		HeliTest_SIB trans = null;
		HumanCommandVehicle vehCommand = m_Player.GetCommand_Vehicle();
		if (vehCommand )
		{
		if (Class.CastTo(trans,vehCommand.GetTransport()) )
		{
			 return;
		}
		}
		array<ActionBase> possible_interact_actions = m_AM.GetPossibleActions(InteractActionInput);
		int possible_interact_actions_index = m_AM.GetPossibleActionIndex(InteractActionInput);
		array<ActionBase> possible_continuous_interact_actions = m_AM.GetPossibleActions(ContinuousInteractActionInput);
		int possible_continuous_interact_actions_index = m_AM.GetPossibleActionIndex(ContinuousInteractActionInput);
		
		m_InteractActionsNum = possible_interact_actions.Count();
		if ( m_InteractActionsNum > 0 )
		{
			m_Interact = possible_interact_actions[possible_interact_actions_index];
		}
		
		m_ContinuousInteractActionsNum = possible_continuous_interact_actions.Count();
		if ( m_ContinuousInteractActionsNum > 0 )
		{
			m_ContinuousInteract = possible_continuous_interact_actions[possible_continuous_interact_actions_index];
		}
		
		
		m_Single = m_AM.GetPossibleAction(DefaultActionInput);
		m_Continuous = m_AM.GetPossibleAction(ContinuousDefaultActionInput);
	}
};

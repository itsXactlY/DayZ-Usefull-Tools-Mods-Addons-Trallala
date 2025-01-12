modded  class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap); 
		AddAction(ActionHeliStartEngineSIB, InputActionMap);
		AddAction(ActionHeliStopEngineSIB, InputActionMap);
		AddAction(ActionHeliEnableAutopilotSIB, InputActionMap);
		AddAction(ActionHeliDisableAutopilotSIB, InputActionMap);
		
		AddAction(ActionHelicopterSwitchSeatsSIB, InputActionMap);
	}
}


modded class ActionCarDoors
{
  override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
  {  
   CarScript m_Car_sib  = null;
    if ( (Class.CastTo(m_Car_sib, target.GetParent()) || Class.CastTo(m_Car_sib, target.GetObject())) && m_Car_sib.m_typeVehicle > 0)
    {
  //start
  if ( player && player.GetCommand_Vehicle() )
    {
    Transport myTransport = Transport.Cast(player.GetCommand_Vehicle().GetTransport());

      if (myTransport && myTransport != m_Car_sib)
      {
      return false; 
      }
    }
    //end
      CarDoor m_Car_sibDoorX = CarDoor.Cast(target.GetObject());
      if (m_Car_sibDoorX)
      {  
        array<string> selections = new array<string>();
        m_Car_sibDoorX.GetActionComponentNameList(target.GetComponentIndex(), selections);
        for (int i = 0; i < selections.Count(); i++)
        {
         string m_AnimSource_sib = m_Car_sib.GetAnimSourceFromSelection( selections[i]);
          if ( m_AnimSource_sib != "" )
          {
            int idx = m_Car_sib.GetSeatIndexFromDoor(m_AnimSource_sib);
            if (idx != -1 && !m_Car_sib.IsAreaAtDoorFree( idx ))return false;
            m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
            float animationPhase = m_Car_sib.GetAnimationPhase(m_AnimSource_sib);  
            return ( m_IsOpening && animationPhase <= 0.5 ) || ( !m_IsOpening && animationPhase > 0.5 );
          }
        }
      }
      return false;  
    }
    return super.ActionCondition( player, target,item );  
  }
}
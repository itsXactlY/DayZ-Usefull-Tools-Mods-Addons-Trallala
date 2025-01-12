modded class ActionGetOutTransport
{
  override void Start( ActionData action_data )
  {
    
    HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
    if( vehCommand )
    {
      Transport trans = vehCommand.GetTransport();
      if ( trans )
      {
        CarScript car;
        if ( Class.CastTo(car, trans) && car.m_typeVehicle == 1)
        {
          float anim_engine = car.GetAnimationPhase("hide_start");
          float anim_rotor =  car.GetAnimationPhase("hide_rotor");
          if(anim_engine == 1.0 && anim_rotor == 0.0){car.SetAnimationPhase("hide_start", 0.0);}            
        }
      }
    }
  super.Start( action_data );
  }
};
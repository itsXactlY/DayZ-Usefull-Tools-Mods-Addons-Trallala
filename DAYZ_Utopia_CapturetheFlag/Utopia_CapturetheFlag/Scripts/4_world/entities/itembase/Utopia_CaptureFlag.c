class Utopia_CaptureFlag_Base extends Clothing
{
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
			
		SetIsPlaceSound( true );
	}
	
	override string GetPlaceSoundset()
	{
		return "placeBarrel_SoundSet";
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override bool IsTwoHandedBehaviour() 
    {
        return true;
    }
	
    override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPlaceObject);
	}

	override bool CanPutInCargo( EntityAI parent )
    {
        return false;
    }
    override bool CanPutIntoHands( EntityAI parent )
    {
        return true;
    }
}
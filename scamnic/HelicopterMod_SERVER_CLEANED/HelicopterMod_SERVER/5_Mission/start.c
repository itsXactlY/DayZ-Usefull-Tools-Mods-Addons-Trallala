// SIB MOD v0.1
modded class MissionServer
{
	void HelicopterStartSimulate(ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{
		Param2<EntityAI,EntityAI> infoHeli = new Param2<EntityAI,EntityAI>(NULL,NULL);
		ctx.Read(infoHeli);
		EntityAI vert = infoHeli.param1;
		EntityAI pilot = infoHeli.param2;
		HeliTest_SIB helicopter = HeliTest_SIB.Cast(vert);
		if(helicopter)
		{
			float anim_engine = helicopter.GetAnimationPhase("hide_start");
			if(anim_engine == 1.0)
			{
				helicopter.SetAnimationPhase("hide_rotor", 1.0);
				animatezzstart(helicopter);
			}
		}
	}
}


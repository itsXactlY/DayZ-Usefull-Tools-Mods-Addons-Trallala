modded class MissionServer
{
	void MissionServer()
	{
		GetRPCManager_SMOD().Reg_Class(this);
	}	
}
modded class MissionGameplay
{
	void MissionGameplay()
	{	
		GetRPCManager_SMOD().Reg_Class(this);	
	}
};
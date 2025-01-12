protected const int FRAMEWORK_RPC_ID2 = 1004257;

class RPCManager_SMOD
{
	protected Class m_InstanceClass;	

	void RPCManager_SMOD()
	{
		GetDayZGame().Event_OnRPC.Insert( OnRPC );
	}
	
	void ~RPCManager_SMOD()
	{
		GetDayZGame().Event_OnRPC.Remove( OnRPC );
	}
	
	void Reg_Class(Class instance)
	{
		
		m_InstanceClass = instance;
	}
	
	Class GetInstanceClass() 
	{
		return m_InstanceClass;
	}

	void OnRPC( PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx )
	{
		if ( rpc_type != FRAMEWORK_RPC_ID2 )
		{
			return;
		}

		ref Param1<string> functionName = new ref Param1<string>("");
		if ( !ctx.Read( functionName ) )
		{
			Print("RPCManager_SMOD||OnRPC||Error functionName");
			return;
		}
		auto functionCallData = new Param3<ref ParamsReadContext, ref PlayerIdentity, ref Object >(ctx, sender, target);
		int result = GetGame().GameScript.CallFunctionParams(GetRPCManager_SMOD().GetInstanceClass(), functionName.param1, NULL, functionCallData);
	}
	void SendRPC(ref array<ref Param> params, bool guaranteed = false, ref PlayerIdentity sendToIdentity = NULL, ref Object sendToTarget = NULL )
	{
		GetGame().RPC( sendToTarget, FRAMEWORK_RPC_ID2, params, guaranteed, sendToIdentity );
	}
}
static ref RPCManager_SMOD g_RPCManager_SMOD;
static ref RPCManager_SMOD GetRPCManager_SMOD()
{
	if ( !g_RPCManager_SMOD )
	{
		g_RPCManager_SMOD = new ref RPCManager_SMOD;
	}
	return g_RPCManager_SMOD;
}
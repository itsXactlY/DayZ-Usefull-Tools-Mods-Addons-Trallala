class CfgPatches
{
	class Utopia_CapturetheFlag_Server
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data", 
			"DZ_Scripts",
			"Utopia_CapturetheFlag", 
			"Utopia_Core",
			"Utopia_CoreServer"
		};
	};
};
class CfgMods
{
	class Utopia_CapturetheFlag_Server
	{
		dir="Utopia_CapturetheFlag_Server";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="Utopia_CapturetheFlag_Server";
		credits="Genesis,Gunnar";
		author="Gunnar";
		authorID="0";
		version="1.3";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"LBmaster_Groups/scripts/Common",
					"BasicMapDefine/scripts/Common",
					"Utopia_CapturetheFlag_Server/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"LBmaster_Groups/scripts/Common",
					"BasicMapDefine/scripts/Common",
					"Utopia_CapturetheFlag_Server/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Utopia_CapturetheFlag_Server/scripts/5_Mission"
				};
			};
		};
	};
};
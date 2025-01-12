class CfgMods
{
	class HelicopterMod_SERVER
	{
		dir = "HelicopterMod_SERVER";
		hideName = 0;
		hidePicture = 0;
		name = "";
		credits = "";
		author = "";
		authorID = "";
		version = "version 0.1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"HelicopterMod_SERVER/4_World"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"HelicopterMod_SERVER/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"HelicopterMod_SERVER/5_Mission"};
			};
		};
	};
};
class CfgPatches
{
	class HelicopterModSERVER
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"HelicopterModScriptsSIB"};
	};
};

class CfgPatches
{
	class Utopia_CoreServer
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "Utopia_Core"};
	};
};

class CfgMods
{
	class Utopia_CoreServer
	{
		dir = "Utopia_CoreServer";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Utopia_CoreServer";
		credits = "Utopia";
		author = "Utopia";
		authorID = "0";
		version = 1.0;
		extra = 0;
		type = "mod";
		dependencies[] = 
		{
			"Game",
			"World",
			"Mission"
		};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {
					"Utopia_CoreServer/Scripts/3_Game"
					};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {
					"Utopia_CoreServer/Scripts/4_World"
					};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"Utopia_CoreServer/Scripts/5_Mission"};
			};
		};
	};
};

class CfgPatches
{
	class Utopia_KOTH
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = 
		{
			"DZ_Data",
			"DZ_Scripts",
			"Utopia_KOTHServer"
		};
	};
};

class CfgMods
{
	class Utopia_KOTH
	{
		dir = "Utopia_KOTH";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Utopia_KOTH";
		credits = "Utopia";
		author = "Utopia";
		authorID = "0";
		version = 1.0;
		extra = 0;
		type = "mod";
		inputs = "";
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
				files[] = {"Utopia_KOTH/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"Utopia_KOTH/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"Utopia_KOTH/Scripts/5_Mission"};
			};
		};
	};
};

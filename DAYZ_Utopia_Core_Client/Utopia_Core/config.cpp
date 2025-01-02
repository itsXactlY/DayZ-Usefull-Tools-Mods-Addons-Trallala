class CfgPatches
{
	class Utopia_Core
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class Utopia_Core
	{
		dir = "Utopia_Core";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Utopia_Core";
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
					"LBmaster_Groups/scripts/Common",
					"BasicMapDefine/scripts/Common",
					"Utopia_Core/Scripts/3_Game"
					};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {
					"LBmaster_Groups/scripts/Common",
					"BasicMapDefine/scripts/Common",
					"Utopia_Core/Scripts/4_World"
					};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"Utopia_Core/Scripts/5_Mission"};
			};
		};
	};
};

class CfgPatches
{
	class Utopia_KOTHServer
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts", "Utopia_Core", "Utopia_CoreServer"};
	};
};

class CfgMods
{
	class Utopia_KOTHServer
	{
		dir = "Utopia_KOTHServer";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Utopia_KOTHServer";
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
				files[] = {"Utopia_KOTHServer/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"Utopia_KOTHServer/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"Utopia_KOTHServer/Scripts/5_Mission"};
			};
		};
	};
};

class CfgSoundShaders
{
    class Utopia_KOTH_SoundShader
    {
        samples[] = {{"Utopia_KOTH/Sounds/Siren.ogg", 1}};
        volume = 1.0;
        range = 1000;  // Max distance sound can be heard
        rangeCurve[] = {{0, 1}, {1000, 0}};  // Fades sound volume over distance
    };
};

class CfgSoundSets
{
    class Utopia_KOTH_SoundSet
    {
        soundShaders[] = {"Utopia_KOTH_SoundShader"};
        volumeFactor = 1.0;
        spatial = 1;  // Makes the sound 3D positional
        doppler = 0;  // No doppler effect
        loop = 0;     // No loop
    };
};

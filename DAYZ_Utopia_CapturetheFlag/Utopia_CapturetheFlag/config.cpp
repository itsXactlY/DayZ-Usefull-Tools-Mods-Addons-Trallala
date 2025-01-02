class CfgPatches
{
	class Utopia_CapturetheFlag
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={
			"DZ_Data", 
			"DZ_Scripts",
			"Utopia_Core"
		};
	};
};
class CfgMods
{
	class Utopia_CapturetheFlag
	{
		dir="Utopia_CapturetheFlag";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="Utopia_CapturetheFlags";
		credits="Utopia";
		author="Genesis";
		authorID="76561198273516768";
		version="0.1";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"World"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Utopia_CapturetheFlag/Scripts/4_world"
				};
			};
		};
	};
};
class CfgPatches
{
	class Custom_Proxies
	{
		units[]=
		{
			"CaptureFlag"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[] = 
		{
			"DZ_Data"
		};
	};
};

class CfgSlots {
	class Slot_CaptureFlag
	{
		name = "CaptureFlag";
		displayName = "Filament";
		ghostIcon = "set:CaptureTheFlag image:CaptureFlag";
	};
};

class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyCaptureFlag: ProxyAttachment
	{
		inventorySlot[] = {"CaptureFlag"};
		model = "Utopia_CapturetheFlag\Models\Proxies\CaptureFlag.p3d";
	};
};
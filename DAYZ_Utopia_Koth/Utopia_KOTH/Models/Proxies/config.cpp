class CfgPatches
{
	class Custom_Proxies
	{
		units[]=
		{
			"Koth_Laptop_Battery",
			"Koth_Satcom_Dish"
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
	class Slot_Koth_Laptop_Battery
	{
		name = "Koth Laptop Battery";
		displayName = "Koth Laptop Battery";
		ghostIcon = "set:Koth_Battery image:Battery";
	};
	class Slot_Koth_Satcom_Dish {
		name = "Koth_Satcom_Dish";
		displayName = "Koth Satcom Dish";
		ghostIcon = "set:Koth_Satcom_Dish image:Satcom_Dish";
	};
};

class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyKoth_Laptop_Battery: ProxyAttachment
	{
		inventorySlot[] = {"Koth_Laptop_Battery"};
		model = "Utopia_KOTH\Models\Proxies\Koth_Laptop_Battery.p3d";
	};
	class ProxyKoth_Satcom_Dish: ProxyAttachment
	{
		inventorySlot[] = {"Koth_Satcom_Dish"};
		model = "Utopia_KOTH\Models\Proxies\Satcom_Dish.p3d";
	};
};
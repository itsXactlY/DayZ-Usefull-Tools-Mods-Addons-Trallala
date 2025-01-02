class CfgPatches
{
	class Utopia_CapturetheFlag
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class Clothing;
	class Container_Base;
	class Utopia_Flag_Base: Container_Base
	{
		scope=2;
		model="Utopia_CapturetheFlag\Models\CaptureFlag_Base.p3d";
		displayName = "CaptureFlag";
		descriptionShort = "";
		itemSize[] = {10,10};
		weight = 500;
		rotationFlags = 17;
		hiddenSelections[]=
		{
			"camo1",
			"camo2"
		};
		hiddenSelectionsTextures[]=
		{
			"Utopia_CapturetheFlag\Models\Textures\sandbags_co.paa",
			"Utopia_CapturetheFlag\Models\Textures\sandbags_co.paa"
		};
        attachments[] = 
		{
			"CaptureFlag"
		};
		class GUIInventoryAttachmentsProps
		{
			class Flag
			{
				name="Flag";
				description="";
				attachmentSlots[]=
				{
					"CaptureFlag"
				};
				icon="set:dayz_inventory image:cat_common_cargo";
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 50;
					healthLevels[]= {{1,{}},{0.69999999,{}},{0.5,{}},{0.30000001,{}},{0,{}}};
				};
			};
		};
	};
	class Utopia_CaptureFlag_Base: Clothing
	{
		scope = 0;
		displayName = "CaptureFlag";
		descriptionShort = "";
		weight = 8000;
		itemSize[] = {10,120};
		absorbency = 0.1;
		heatIsolation = 0.1;
		physLayer = "item_large";
		model = "Utopia_CapturetheFlag\Models\Flag\CaptureFlag_G.p3d";
		inventorySlot[] =
		{
			"Armband",
			"CaptureFlag"
		};
		hiddenSelections[] = 
		{
			"camo1",
			"camo2"
		};
		class ClothingTypes
		{
			male = "\Utopia_CapturetheFlag\Models\Flag\CaptureFlag_M.p3d";
			female = "\Utopia_CapturetheFlag\Models\Flag\CaptureFlag_M.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 1000000;
				};
			};
		};
	};
	class Utopia_CaptureFlag: Utopia_CaptureFlag_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] = 
		{
			"Utopia_CapturetheFlag\Models\Textures\sandbags_co.paa",
			"utopia_capturetheflag\models\textures\flag_co.paa"
		};
	};
};
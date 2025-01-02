class CfgPatches
{
	class Utopia_KOTH
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgVehicles
{
	class Container_Base;
	class Inventory_Base;
	class HouseNoDestruct;
	class KothPoint: Container_Base
	{
		scope=2;
		displayName="Utopia Koth";
		descriptionShort="KOTH Used to call in supplys and gear.";
		model="Utopia_KOTH\Models\Military_Desk\Military_Desk.p3d";
		slopeTolerance=0.40000001;
		yawPitchRollLimit[]={45,45,45};
		itemBehaviour=2;
		rotationFlags=2;
		hiddenSelections[] = 
		{
			"camo"
		};
		hiddenSelectionsTextures[] =
		{
			"Utopia_KOTH\Models\Textures\Desk_co.paa"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[]= {{1,{}},{0.69999999,{}},{0.5,{}},{0.30000001,{}},{0,{}}};
				};
			};
		};
	};
	class Utopia_KOTH_Siren_Base: HouseNoDestruct
	{
		scope = 0;
		model = "Utopia_KOTH\Models\Siren\Utopia_Siren.p3d";
		displayName = "KOTH Siren";
		descriptionShort = "";
		hiddenSelections[] = 
		{
			"camo"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[]= {{1,{}},{0.69999999,{}},{0.5,{}},{0.30000001,{}},{0,{}}};
				};
			};
		};
	};
	class Utopia_KOTH_Siren_Green: Utopia_KOTH_Siren_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] =
		{
			"Utopia_KOTH\Models\Textures\Siren_Green.paa"
		};
	};
	class Utopia_KOTH_Siren_Red: Utopia_KOTH_Siren_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] =
		{
			"Utopia_KOTH\Models\Textures\Siren_Red.paa"
		};
	};
	class Utopia_KOTH_Siren_Blue: Utopia_KOTH_Siren_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] =
		{
			"Utopia_KOTH\Models\Textures\Siren_Blue.paa"
		};
	};
	class Utopia_KOTH_Siren_Pink: Utopia_KOTH_Siren_Base
	{
		scope = 2;
		hiddenSelectionsTextures[] =
		{
			"Utopia_KOTH\Models\Textures\Siren_Pink.paa"
		};
	};
	class Utopia_Koth_Laptop: Inventory_Base
	{
		scope = 2;
		model = "Utopia_KOTH\Models\Laptop\Utopia_Laptop.p3d";
		displayName = "Laptop";
		descriptionShort = "";
		weight = 500;
		itemSize[] = {4,3};
		hiddenSelections[]={"camo","screen","frontlight","keyboard"};
		rotationFlags = 2;
		overrideDrawArea = 3.0;
		carveNavmesh=1;
		forceFarBubble = "true";
		attachments[] = 
		{
			"Koth_Laptop_Battery"
		};
		hiddenSelectionsTextures[]=
		{
			"Utopia_KOTH\Models\Textures\Laptop_co.paa"
		};
		class GUIInventoryAttachmentsProps
		{
			class Koth_Laptop_Battery
			{
				name="Koth Laptop Battery";
				description="";
				attachmentSlots[]=
				{
					"Koth_Laptop_Battery"
				};
				icon="set:dayz_inventory image:cat_common_cargo";
			};
		};
		class Cargo
		{
			itemsCargoSize[]={0,0};
			openable=1;
			allowOwnedCargoManipulation=1;
		};
		class AnimationSources
		{
			class door1
			{
				source="user";
				initPhase=0;
				animPeriod=1.5;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[]= {{1,{}},{0.69999999,{}},{0.5,{}},{0.30000001,{}},{0,{}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
		};
	};
	class Utopia_Koth_Satcom: Container_Base
	{
		scope=2;
		displayName="Utopia Koth Laptop";
		descriptionShort="KOTH Laptop Used to call in supplys and gear.";
		model="Utopia_KOTH\Models\Laptop\Utopia_Laptop.p3d";
		slopeTolerance=0.40000001;
		yawPitchRollLimit[]={45,45,45};
		itemBehaviour=2;
		rotationFlags=2;
		hiddenSelections[] = 
		{
			"camo"
		};
		hiddenSelectionsTextures[] =
		{
			"Utopia_KOTH\Models\Textures\laptop_co.paa"
		};
		class GUIInventoryAttachmentsProps
		{
			class Battery
			{
				name="Battery";
				description="";
				attachmentSlots[]=
				{
					"BatteryD"
				};
				icon="cat_common_cargo";
			};
		};
		class Cargo
		{
			itemsCargoSize[]={0,0};
			openable=0;
			allowOwnedCargoManipulation=1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[]= {{1,{}},{0.69999999,{}},{0.5,{}},{0.30000001,{}},{0,{}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.01;
			plugType = 1;
			attachmentAction = 1;
			updateInterval=4;
		};
	};
	class Utopia_Koth_Laptop_Battery: Inventory_Base
	{
		scope = 2;
		model = "Utopia_KOTH\Models\Laptop\Laptop_Battery.p3d";
		displayName = "Koth Laptop Battery";
		descriptionShort = "";
		weight = 500;
		itemSize[] = {2,2};
		hiddenSelections[]={"camo"};
		rotationFlags = 2;
		overrideDrawArea = 3.0;
		carveNavmesh=1;
		forceFarBubble = "true";
		stackedUnit = "w";
		quantityBar = 1;
		varQuantityInit = 50;
		varQuantityMin = 0.0;
		varQuantityMax = 50;
		varQuantityDestroyOnMin = 0;
		inventorySlot[]=
		{
			"Koth_Laptop_Battery"
		};
		hiddenSelectionsTextures[]=
		{
			"Utopia_KOTH\Models\Textures\Laptop_co.paa"
		};
		class Cargo
		{
			itemsCargoSize[]={0,0};
			openable=0;
			allowOwnedCargoManipulation=1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[]= {{1,{}},{0.69999999,{}},{0.5,{}},{0.30000001,{}},{0,{}}};
				};
			};
		};
		class EnergyManager
		{
			hasIcon = 1;
			switchOnAtSpawn = 1;
			isPassiveDevice = 1;
			energyStorageMax = 50;
			energyAtSpawn = 50;
			convertEnergyToQuantity = 1;
			reduceMaxEnergyByDamageCoef = 0;
			powerSocketsCount = 1;
			compatiblePlugTypes[] = {1};
		};
	};
};

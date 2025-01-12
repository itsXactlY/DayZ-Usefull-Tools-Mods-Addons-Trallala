
class Json_Heli
{
	float version = 5.1;
	float duration = 0.050000;
	float Lift_power = 3.000000;
	float No_power = 700.000000;
	float Full_power = 750.000000;
	float side_strength = 1.300000;
	float power_before = 1.000000;
	float friction = 0.300000;
	float Mass = 1500.812134;
	float incline=1.0;
	float front_tilt=1.5;
	float inclineMouse = 2.5;
	float front_tiltMouse = 2.5;
	float m_povorot = 0.5;
	
	
	float Chassis=0;
	float Body=1;
	float Engine=1;
	float Fuel=1;
	float Rotor1=1;
	float Rotor2=1;
	
	
	
	
	float kill_z=0;

	float gasoline_costs=0.0005;

	float gasoline_restart=1;
	float c_camDistance=10;
	float maxFuel = 30.0;
	float m_Crash_dem = 15.0;
	
	float m_engineVolume=1.0;
	float m_rotorVolume=1.0;
	float m_warningVolume=1.0;
	float m_crashVolume=1.0;
	
	
};




modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		conf_start();
		descr();
	}
	void conf_start()
	{
		string pathzz = "cfgVehicles";string child_name = ""; 
		int countxx =  GetGame().ConfigGetChildrenCount ( pathzz );
		for (int p = 0; p < countxx; p++)
		{
			GetGame().ConfigGetChildName ( pathzz, p, child_name );
			if (GetGame().ConfigGetInt(pathzz + " " + child_name + " scope") == 2 && GetGame().IsKindOf(child_name,"HeliTest_SIB"))
			{
				vert_conf(child_name);
			}
		}
		
	}
	void obnul_conf(bool obnul,string child_name)
	{
		if(obnul)
		{
			string papka =  "$profile:\\Heli_sib"; 
			if (!FileExist(papka))
			{
				MakeDirectory(papka);
			}
			ref Json_Heli config_heli_clear  = new Json_Heli();
			string path =  "$profile:\\Heli_sib\\"+child_name+"_config.json";   
			JsonFileLoaderVert<ref Json_Heli>.JsonSaveFile(path, config_heli_clear);
			vert_conf(child_name);
		}
	}
	void vert_conf(string child_name)
	{
		bool obnul=true;
		string papka =  "$profile:\\Heli_sib"; 
		if (!FileExist(papka))
		{
			MakeDirectory(papka);
		}
		ref Json_Heli config_heli  = new Json_Heli();
		float version_json_base = config_heli.version;
		string path =  "$profile:\\Heli_sib\\"+child_name+"_config.json";   
		if (!FileExist(path))
		{
			JsonFileLoaderVert<ref Json_Heli>.JsonSaveFile(path, config_heli);
		}
		JsonFileLoaderVert<ref Json_Heli>.JsonLoadFile(path, config_heli);
		JsonFileLoaderVert<ref Json_Heli>.JsonSaveFile(path, config_heli);
		int num_mass=0;
		float version_json_config = config_heli.version;
		if(version_json_config<version_json_base || version_json_config == 0)
		{
			obnul_conf(true,child_name);
			return;
		}
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.duration );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Lift_power );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.No_power );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Full_power );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.side_strength );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.power_before );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.friction );num_mass++;
		if(config_heli.Mass > 1)obnul=false;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Mass );num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.incline);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.front_tilt);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.inclineMouse);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.front_tiltMouse);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.m_povorot);num_mass++;

		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Chassis);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Body);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Engine);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Fuel);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Rotor1);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.Rotor2);num_mass++;
		
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.kill_z);num_mass++;

		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.gasoline_costs);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.gasoline_restart);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.c_camDistance);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.maxFuel);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.m_Crash_dem);num_mass++;
		if(config_heli.m_engineVolume == 0)obnul=true;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.m_engineVolume);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.m_rotorVolume);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.m_warningVolume);num_mass++;
		ssaBWDQnlAkkAQp.Insert(child_name+"_"+num_mass, config_heli.m_crashVolume);num_mass++;
		obnul_conf(obnul,child_name);
	}
	
	void descr()
	{
		string path =  "$profile:\\Heli_sib\\description_config.json";  
		FileHandle handle1 = OpenFile( path, FileMode.WRITE );

		if ( handle1 == 0 )
		{
			return;
		}
		string distr_std="";
		distr_std =distr_std+"//////////ENG///////\n";
		distr_std =distr_std+"\n";
		distr_std =distr_std+"    \"duration\": 0.05_, - rotor_ spin speed_\n";
		distr_std =distr_std+"    \"Lift_power\": 1.5_, - air resistance level - for large_ helicopters there should be more\n";
		distr_std =distr_std+"    \"No_power\": 700_, - how much the helicopter loses traction compared to Full_power\n";
		distr_std =distr_std+"    \"Full_power\": 750_, - the maximum height when the helicopter loses its thrust\n";
		distr_std =distr_std+"    \"side_strength\": 1.3_, - main rotor_ erosion\n";
		distr_std =distr_std+"    \"power_before\": 1_, - power of the rear rotor_\n";
		distr_std =distr_std+"    \"friction\": 0.3_, - friction coefficient\n";
		distr_std =distr_std+"    \"Mass\": 1500.81_, - weight\n";
		distr_std =distr_std+"    \"incline\": 0.5_, - side inclination speed_\n";
		distr_std =distr_std+"    \"front_tilt\": 0.5_, - tilt in front and back\n";
		distr_std =distr_std+"    \"inclineMouse\": 2.5_, - side inclination speed_Mouse\n";
		distr_std =distr_std+"    \"front_tiltMouse\": 2.5_, - tilt in front and back_Mouse\n";
		distr_std =distr_std+"    \"m_povorot\": 0.5_, -  rotation  speed_\n";
		
		
		
		distr_std =distr_std+"    \"Chassis\": 0_, example 0-1- damage_ to the wheels or the stand\n";
		distr_std =distr_std+"    \"Body\": 0_, example 0-1- nose damage_\n";
		distr_std =distr_std+"    \"Engine\": 1_, example 0-1- engine damage_\n";
		distr_std =distr_std+"    \"Fuel\": 0_, example 0-1 damage_ Fuel \n";
		distr_std =distr_std+"    \"Rotor1\": 0_, example 0-1- damage_ to the main rotor_\n";
		distr_std =distr_std+"    \"Rotor2\": 0_, example 0-1- damage_ to the rotor_ 2\n";
		
		
		distr_std =distr_std+"    \"kill_z\": 0_, example 0-1- deal damage_ to players with damage_ from a blow\n";
		distr_std =distr_std+"    \"gasoline_costs\": 0.001_, - gasoline_ waste\n";
		distr_std =distr_std+"    \"gasoline_restart\": 1_,  example 0-1- automatic_ refueling at restart\n";
		distr_std =distr_std+"    \"c_camDistance\": 10_,  example 0.1-100 - distance camera\n";
		distr_std =distr_std+"    \"maxFuel\": 30_,  example 0.1-100 - max_ Fuel_\n";
		distr_std =distr_std+"    \"m_Crash_dem\": 15_,  example 0.1-100 - maximum_speed at which the sound of Crash\n";
		distr_std =distr_std+"    \"m_engineVolume\": 1_,  example 0.001-100 - Volume sound engine\n";
		distr_std =distr_std+"    \"m_rotorVolume\": 1_,  example 0.001-100 -  Volume sound rotor\n";
		distr_std =distr_std+"    \"m_warningVolume\": 1_,  example 0.001-100 - Volume sound warning\n";
		distr_std =distr_std+"    \"m_crashVolume\": 1_,  example 0.001-100 -  Volume sound crash\n";
		

		
		distr_std =distr_std+"\n";
		distr_std =distr_std+"//////////RUS///////\n";
		distr_std =distr_std+"\n";
		distr_std =distr_std+"    \"duration\": 0.05_, - скорость раскрутки ротора\n";
		distr_std =distr_std+"    \"Lift_power\": 1.5_, - уровень сопротивления воздуху - для больших вертолетов должно быть больше\n";
		distr_std =distr_std+"    \"No_power\": 700_, -  на сколько вертолет теряет тягу посравнению с Full_power\n";
		distr_std =distr_std+"    \"Full_power\": 750_,  - максимальная высота когда вертолет теряет поную тягу\n";
		distr_std =distr_std+"    \"side_strength\": 1.3_, - мошность главного ротора\n";
		distr_std =distr_std+"    \"power_before\": 1_, - мощность заднего ротора\n";
		distr_std =distr_std+"    \"friction\": 0.3_, - коофициент трения\n";
		distr_std =distr_std+"    \"Mass\": 1500.81_, - вес \n";
		distr_std =distr_std+"    \"incline\": 0.5_,  - скорость наклона в бок\n";
		distr_std =distr_std+"    \"front_tilt\": 0.5_, - наклон в перед и назад\n";
		distr_std =distr_std+"    \"inclineMouse\": 2.5_, - скорость наклона в бок мышью\n";
		distr_std =distr_std+"    \"front_tiltMouse\": 2.5_, -наклон в перед и назад мышью\n";
		distr_std =distr_std+"    \"m_povorot\": 0.5_, - скорость поворота на 90_ градусов _\n";
		
		distr_std =distr_std+"    \"Chassis\": 0_, 1_ вкл 0_ выкл - колеса\n";
		distr_std =distr_std+"    \"Body\": 0_,  1 вкл 0_ выкл- корпус \n";
		distr_std =distr_std+"    \"Engine\": 1_,  1_ вкл 0_ выкл - движок\n";
		distr_std =distr_std+"    \"Fuel\": 0_,  1 вкл 0 выкл бак \n";
		distr_std =distr_std+"    \"Rotor1\": 0_,  1_ вкл 0_ выкл главный ротор\n";
		distr_std =distr_std+"    \"Rotor2\": 0_,  1_ вкл 0_ выкл ротор 2_ \n";
		
		distr_std =distr_std+"    \"kill_z\": 0_, пример 1-0 - наносить урон игрокам при повреждении от удара\n";
		distr_std =distr_std+"    \"gasoline_costs\": 0.001_, - трата бензина\n";
		distr_std =distr_std+"    \"gasoline_restart\": 1_,  пример 1-0- автоматическая заправка при рестарте\n";
		distr_std =distr_std+"    \"c_camDistance\": 10_,  пример 0.1-100- дальность камеры\n";
		distr_std =distr_std+"    \"maxFuel\": 10_  пример 0.1-100- макс бензина\n";
		distr_std =distr_std+"    \"m_Crash_dem\": 15_  пример 0.1-100- максимальная скорость при котором будет звук урона\n";
		distr_std =distr_std+"    \"m_engineVolume\": 1_,  example 0.001-100 - уровень звука engine\n";
		distr_std =distr_std+"    \"m_rotorVolume\": 1_,  example 0.001-100 -  уровень звука rotor\n";
		distr_std =distr_std+"    \"m_warningVolume\": 1_,  example 0.001-100 - уровень звука warning\n";
		distr_std =distr_std+"    \"m_crashVolume\": 1_,  example 0.001-100 -  уровень звука crash\n";
		FPrint( handle1, distr_std ); 
		CloseFile( handle1);
	}



};









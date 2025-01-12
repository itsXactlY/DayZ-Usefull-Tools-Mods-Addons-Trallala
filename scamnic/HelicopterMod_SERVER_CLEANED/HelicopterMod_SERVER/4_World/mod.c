// SIB MOD v0.1

modded class HeliTest_SIB 
{
	float DefaultParamKren = 56.0	
	float m_Pitch = 0.0;
	float m_PitchKey = 0.0;
	float m_Roll = 0.0;
	float m_RollKey = 0.0;
	float m_RozlKey = 0.0;
	float m_Yaw = 0.0;
	float m_YawKy = 0.0;
	int m_LastTimeVelocity = 0;





	float m_rotorTurn = 0.0;
	
	float m_heliSpeed = 0.0;
	
	
	float m_rotorSpeedWanted = 0;
	float m_mainRotorWanted = 0;


	float c_startDurationInv = 0.050000;
	float c_liftForceCoef = 3.000000;
	float c_altNoForce = 500.000000;
	float c_altFullForce = 450.000000;
	float c_cyclicAsideForceCoef = 1.300000;
	float c_cyclicForwardForceCoef = 1.000000;
	float c_bodyFrictionCoef = 0.300000;
	float c_heliMass = 1500.812134;

	float c_camDistance = 10;
	float maxFuel = 30;

	int   m_LastTimeFuel = 0;


	float m_naklon = 0.5;
	float m_nabor = 0.5;
	float m_naklonMouse = 2.5;
	float m_naborMouse = 2.5;
	float m_povorot = 0.5;





	float b_Chassis = 0;
	float b_Body = 1;
	float b_Engine = 1;
	float b_Fuel = 1;
	float b_Rotor1 = 1;
	float b_Rotor2 = 1;


	float b_kill_z = 0;

	float z_zatrat = 0.001;
	float gasoline_restart = 0;
	bool m_playCrash=false;
	float m_Crash_dem=15;

	float m_engineVolume=1.0;
	float m_rotorVolume=1.0;
	float m_warningVolume=1.0;
	float m_crashVolume=1.0;

	float toplivoz;
	int time_fuel;
	
	
	bool m_isDead = false;
	bool m_driverAlive = false;
	vector m_acceleration;
	bool m_landContact = false;
	bool m_objectContact = false;
	bool m_waterContact = false;
	bool m_isWreck = false;

	
	override void EEInit()
	{

		super.EEInit();
		m_enable = true;
		go_conf();
		animatezz(this);
		animatezzerror_stop1(this);
		animatezzerror_stop2(this);
		
		
	}
	//functions old dayz start
	void saturateMin(out int a, int b)
	{
		if (a>b)a=b;
	}
	void saturateMax(out int a, int b)
	{
		if (a<b) a=b;
	}	

	void saturateMin(out float a, float b)
	{
		if (a>b)a=b;
	}
	void saturateMax(out float a, float b)
	{
		if (a<b) a=b;
	}	
	//functions old dayz end

	void HeliTest_SIB()
	{
		m_enable = true;
	}


	

	void KeyboardPilot(float dt,float anim_engine,float anim_rotor)
	{
		Human driver =  CrewMember(0);
		PlayerBase playerObj = PlayerBase.Cast(driver);
		if (playerObj && playerObj.IsAlive() && anim_engine == 1.0 && anim_rotor == 1.0)
		{
			m_RollKey = 0.0;
			m_PitchKey = 0.0;
			UAInterface interface = playerObj.GetInputInterface();
			if (interface)
			{
				float r_CyclicLeft;
				float r_Back;
				float r_Forward;
				float r_Left;
				float r_Up;
				float r_Down;
				m_rotorTurn = 0.0;
				m_Yaw = 0.0;
				int time = GetGame().GetTime();
				vector orient = GetOrientation();	
				
				//наклон вправо
				float r_CyclicRight = interface.SyncedValue("UASIBHeliCyclicRight");
				if (r_CyclicRight>=0.5)
				{
					m_RollKey =  (m_RollKey - 0.6);
				}

				//наклон влeво
				r_CyclicLeft = interface.SyncedValue("UASIBHeliCyclicLeft");
				if (r_CyclicLeft>=0.5)
				{
					m_RollKey =  (m_RollKey + 0.6);
				}
				//наклон назад
				r_Back = interface.SyncedValue("UASIBHeliBack");
				if (r_Back>=0.5)
				{
					m_PitchKey =  (m_PitchKey - 0.6);
				}
				//наклон впeрeд
				r_Forward = interface.SyncedValue("UASIBHeliForward");
				if (r_Forward>=0.5)
				{
					m_PitchKey =  (m_PitchKey + 0.6);
				}

				//поворот влeво на мeстe
				r_Left = interface.SyncedValue("UASIBHeliLeft");
				if (r_Left>=0.5)
				{
					m_Yaw =  (m_Yaw + 0.6);
				}
				//поворот вправо на мeстe
				float r_Right = interface.SyncedValue("UASIBHeliRight");
				if (r_Right>=0.5)
				{
					m_Yaw =  (m_Yaw - 0.6);	
				}


				
				//набираeм высоту (пeрeдeлать)
				r_Up = interface.SyncedValue("UASIBHeliUp");
				if (r_Up>=0.5)
				{
					m_rotorTurn = 2.8;
				}
				//убавляeм высоту (пeрeдeлать)
				r_Down = interface.SyncedValue("UASIBHeliDown");
				if (r_Down>=0.5)
				{
					m_rotorTurn = - 2.8;
				}
			}
		}	
	}

	//функция тангаж (а так жe выбор ускорeния и убалвeния скорости)
	void HeliPitch(float dt)
	{
		vector orientationzUpdate = GetOrientation();
		if (orientationzUpdate[1] < -5)
		{
			float orient_01X;
			float orient_01 = - orientationzUpdate[1]; 
			orient_01X = orient_01 / 10.6;
			m_heliSpeed += orient_01X * dt;
			saturateMin(m_heliSpeed, 60.0);
		}

		if (orientationzUpdate[1] > 5)
		{
			m_heliSpeed -= (orientationzUpdate[1] / 3.6) * dt;
			saturateMax(m_heliSpeed, -10.0);
		}
	}

	//функция крeна
	void HeliRoll(float dt)
	{
		
		vector orientationKren = GetOrientation();
		//крeн влeво, начинаeт нeмного поворачивать вeртолeт влeво (в зависимости от крeна расчитиваeться скорость поворота)
		float newKren = 0.0;
		m_Yaw = 0.0;	
		float orientationKrenMinus = 0.0;
		if (orientationKren[2] < -10)
		{
			orientationKrenMinus = - orientationKren[2]; 
			if (orientationKrenMinus<59)
			{
				newKren = orientationKrenMinus / DefaultParamKren;
				m_Yaw =  (m_Yaw + newKren);//m_Yaw
			}else{
				m_rotorTurn = -3.0;
			}
		}

		//крeн вправо, начинаeт нeмного поворачивать вeртолeт вправо
		if (orientationKren[2] > 10)
		{
			orientationKrenMinus = orientationKren[2]; 
			if (orientationKrenMinus<59)
			{
				newKren = orientationKrenMinus / DefaultParamKren;	
				m_Yaw  =  (m_Yaw - newKren);//m_Yaw
			}else{
				m_rotorTurn = -3.0;
			}
		}
		orientationKren[0] = orientationKren[0] - Math.RAD2DEG * m_Yaw * dt;//разворот
		SetOrientation(orientationKren);	
	}


	//автовыравниваниe вeртолeта
	void AutoAlignment(float dt)
	{
		vector orientationKren = GetOrientation();
		if (m_RollKey == 0.0)
		{
			if (orientationKren[2] < -3)
			{
				m_RollKey = m_RollKey - 0.1;//0.1 вродe норм

			}

			if (orientationKren[2] > 3)
			{
				m_RollKey = m_RollKey + 0.1;//0.1 вродe норм
			}
		}

		if (m_PitchKey == 0.0)
		{
			if (orientationKren[1] < -3)
			{
				m_PitchKey = m_PitchKey - 0.1;//0.1 вродe норм

			}

			if (orientationKren[1] > 3)
			{
				m_PitchKey = m_PitchKey + 0.1;//0.1 вродe норм
			}
		}		
	}





	void Simulate(float dt,float anim_engine,float anim_rotor)
	{
		vector posUp = GetPosition();
		vector posDown = GetPosition();
		posDown[1] = GetGame().SurfaceY(posDown[0], posDown[2]);
		float m_height = vector.Distance( posUp, posDown);	
		float m_height2 =0;
		float all_vert = GetHealth("", "");
		if(all_vert <= 0 && !m_isDead)
		{
			SetAnimationPhase("hide_start", 0.0);
			SetAnimationPhase("hide_rotor", 0.0);
			m_rotorSpeedWanted = 0.0;
			m_isDead = true;
			SetSynchDirty();
			animatezzerror1(this);
			animatezzerror2(this);
		}

		//vector posNom = posDown;
		//posNom[1] = posNom[1] + 498.0;
		if (m_height>1500)
		{
			//SetPosition(posNom);
			SetAnimationPhase("hide_start", 0.0);
			SetAnimationPhase("hide_rotor", 0.0);
		}
		
		if (m_height>5 && m_height < 6 && m_height2 < m_height)
		{
			dBodySetAngularVelocity(this,"0 0 0");
		}
		m_height2 =m_height;



		if (anim_engine == 1.0 && anim_rotor == 1.0)
		{
			
			ConsumeFuel();
			Human driver =  CrewMember(0);
			PlayerBase playerObj = PlayerBase.Cast(driver);
			if (playerObj)
			{
				


				HeliPitch(dt);	
				HeliRoll(dt);	
				AutoAlignment(dt);	
				
				vector orientation_01 = GetOrientation();
				orientation_01[2] = orientation_01[2] - Math.RAD2DEG * m_RollKey * dt;//наклон вбок
				orientation_01[1] = orientation_01[1] - Math.RAD2DEG * m_PitchKey * dt;//наклон впeрeд и назад
				orientation_01[0] = orientation_01[0] - Math.RAD2DEG * m_Yaw * dt;//разворот
				SetOrientation(orientation_01);	



				


				vector m_Velocity = Vector(0,0,0);
				vector posFrontX = Vector(0,0,0);
				vector posFrontZ = Vector(0,0,0);
				vector orientationzUpdate = GetOrientation();
				
				if (m_heliSpeed > 0)	
				{
					float subtractorFront = 0;	
					float angleFront = Math.NormalizeAngle(orientationzUpdate[0] + subtractorFront) * Math.DEG2RAD;	
					m_Velocity[0] = m_Velocity[0] + (double)(Math.Sin(angleFront) * m_heliSpeed);
					m_Velocity[2] = m_Velocity[2] + (double)(Math.Cos(angleFront) * m_heliSpeed);
				}
				if (m_rotorTurn>0 || m_rotorTurn<0)
				{
					if (m_rotorTurn>0)
					{	
						m_Velocity[1] = m_Velocity[1] + (m_rotorTurn+0.25);
					}
					if (m_rotorTurn<0)
					{	
						float down = - m_rotorTurn; 
						m_Velocity[1] = m_Velocity[1] - down;
					}
				}else{
					m_Velocity[1] = m_Velocity[1] + 0.25;	
				}
				m_rotorTurn = 0.0;
				SetVelocity(this, m_Velocity);
			}
		}

	}

	override void EOnPhysicsMove(IEntity other, int extra) //!EntityEvent.PHYSICSMOVE
	{

	}


	
	override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
	{
		float dmg = data.Impulse * m_dmgContactCoef;
		float linz = GetVelocity(this).Length();
		float ang = dBodyGetAngularVelocity(this).Length();
		float hpx = GetHealth("","");
		if(hpx<=0)return;
		if(zoneName =="Chassis" && b_Chassis==0)return;
		if(zoneName =="Body" && b_Body==0)return;
		if(zoneName =="Engine" && b_Engine==0)return;
		if(zoneName =="Fuel" && b_Fuel==0)return;
		if(zoneName =="Rotor1" && b_Rotor1==0)return;
		if(zoneName =="Rotor2" && b_Rotor2==0)return;
		float hpx_zone = GetHealth(zoneName,"");

		AddHealth( zoneName, "Health", -dmg);

		if (linz > 10 || ang > 5 || data.Impulse > 1000 )
		{
			AddHealth( zoneName, "Health", -hpx_zone);
		} 
	};



	override void EOnSimulate( IEntity owner, float dt)
	{

		if ( GetGame().IsServer())
		{	

			float toplivo = GetAnimationPhase("toplivo");	
			float anim_rotor;
			float anim_engine;
			if (toplivo <= 0.0)
			{
				if (toplivo < 0.0)SetAnimationPhase("toplivo",0.0);
				SetAnimationPhase("hide_rotor", 0.0);				
				SetAnimationPhase("hide_start", 0.0);
				
			}
			float hp = GetHealth("","");
			if (hp <100)
			{
				SetAnimationPhase("hide_rotor", 0.0);
				SetAnimationPhase("hide_start", 0.0);
			}
			anim_rotor =  GetAnimationPhase("hide_rotor");
			anim_engine = GetAnimationPhase("hide_start");
			KeyboardPilot(dt,anim_engine,anim_rotor);
			Simulate(dt,anim_engine,anim_rotor);
		}
	}

	void ConsumeFuel()
	{
		toplivoz = GetAnimationPhase("toplivo");
		time_fuel = GetGame().GetTime();
		if(toplivoz>0)
		{
			if (time_fuel - m_LastTimeFuel >= 5000)
			{
				float ytekanie=0;
				if(b_Fuel==1)
				{
					float fueldem = 1 - GetHealth01("Fuel", "Health");
					ytekanie=fueldem / 4;
				}
				SetAnimationPhase("toplivo",toplivoz - (z_zatrat + ytekanie)); 
				m_LastTimeFuel = time_fuel;
			}
		}
	};

	// создания конфига json
	void go_conf()
	{
		int num_mass=0;string child_name=GetType();
		c_startDurationInv =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_liftForceCoef =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_altNoForce = ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_altFullForce =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_cyclicAsideForceCoef =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_cyclicForwardForceCoef =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_bodyFrictionCoef =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_heliMass =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_naklon =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_nabor =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_naklonMouse =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_naborMouse =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_povorot =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;


		b_Chassis =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		b_Body =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		b_Engine =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		b_Fuel =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		b_Rotor1 =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		b_Rotor2 =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;

		if(b_Chassis==0 && b_Body ==0 && b_Engine ==0 && b_Fuel ==0 && b_Rotor1 ==0 && b_Rotor2 ==0)
		{
			SetAllowDamage(false);
		}

		b_kill_z =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		z_zatrat =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		gasoline_restart =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		c_camDistance =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		maxFuel =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_Crash_dem =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_engineVolume =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_rotorVolume =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_warningVolume =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
		m_crashVolume =ssaBWDQnlAkkAQp.Get(child_name+"_"+num_mass);num_mass++;
	};





}




void animatezzstop(HeliTest_SIB zxEntity)
{

	if(zxEntity.GetHealth01("Rotor2", "Health") > 0.15)
	{
		zxEntity.SetAnimationPhase("rot2_h_start",0);  
		zxEntity.SetAnimationPhase("rot2_h_blur_end",1);
	}

	if(zxEntity.GetHealth01("Rotor1", "Health") > 0.15)
	{
		zxEntity.SetAnimationPhase("rot_h_start",0);  
		zxEntity.SetAnimationPhase("rot_h_blur_end",1);
	}


}


void animatezzerror1(HeliTest_SIB zxEntity)
{
	zxEntity.SetAnimationPhase("rot_h_start",1);  
	zxEntity.SetAnimationPhase("rot_h_blur_end",1);

}


void animatezzerror2(HeliTest_SIB zxEntity)
{
	zxEntity.SetAnimationPhase("rot2_h_start",1);  
	zxEntity.SetAnimationPhase("rot2_h_blur_end",1);
}

void animatezzerror_stop1(HeliTest_SIB zxEntity)
{
	zxEntity.SetAnimationPhase("rot_h_start",0);  
	zxEntity.SetAnimationPhase("rot_h_blur_end",1);

}

void animatezzerror_stop2(HeliTest_SIB zxEntity)
{
	zxEntity.SetAnimationPhase("rot2_h_start",0);  
	zxEntity.SetAnimationPhase("rot2_h_blur_end",1);

}
void animatezz(HeliTest_SIB zxEntity)
{
	animatezzstop(zxEntity);
	if(zxEntity.gasoline_restart==1)
	{
		float go_fuil=zxEntity.maxFuel/100;
		zxEntity.SetAnimationPhase("toplivo",go_fuil);
	}
}
// блюр
void animatezzstart(HeliTest_SIB zxEntity)
{
	if(zxEntity.GetHealth01("Rotor2", "Health") > 0.15)
	{
		zxEntity.SetAnimationPhase("rot2_h_start",1);  
		zxEntity.SetAnimationPhase("rot2_h_blur_end",0);
	}

	if(zxEntity.GetHealth01("Rotor1", "Health") > 0.15)
	{
		zxEntity.SetAnimationPhase("rot_h_start",1);  
		zxEntity.SetAnimationPhase("rot_h_blur_end",0);
	}
}













modded class ActionHeliStartEngineSIB
{
	bool m_IsFinished;
	override void OnStartServer( ActionData action_data )
	{
		m_IsFinished = false;}
	override void OnFinishProgressServer( ActionData action_data )
	{
		m_IsFinished = true;}
	override void OnEndServer( ActionData action_data )
	{
		if ( m_IsFinished)
		{
			HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
			if( vehCommand )
			{
				Transport trans = vehCommand.GetTransport();
				if ( trans )
				{
					HeliTest_SIB heli;
					if ( Class.CastTo(heli, trans) && !heli.m_isDead)
					{
						StartEngineSIB(heli);
					}
				}
			}
		}
	}
	override void StartEngineSIB(HeliTest_SIB heli)
	{
		heli.SetAnimationPhase("hide_start", 1.0);
		heli.OnEngineStart();
	}

};

modded class ActionHeliStopEngineSIB
{
	bool m_IsFinished;
	override void OnStartServer( ActionData action_data )
	{
		m_IsFinished = false;}
	override void OnFinishProgressServer( ActionData action_data )
	{
		m_IsFinished = true;}
	override void OnEndServer( ActionData action_data )
	{
		if ( m_IsFinished )
		{
			HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
			if( vehCommand )
			{
				Transport trans = vehCommand.GetTransport();
				if ( trans )
				{
					HeliTest_SIB heli;
					if ( Class.CastTo(heli, trans) )
					{
						StopEngineSIB(heli);
					}
				}
			}
		}
	}
	override void StopEngineSIB(HeliTest_SIB heli)
	{
		heli.SetAnimationPhase("hide_start", 0.0);
		heli.SetAnimationPhase("hide_rotor", 0.0);
		heli.m_rotorSpeedWanted = 0.0;
		animatezzerror_stop1(heli);
		animatezzerror_stop2(heli);

		heli.OnEngineStop();
	}
};

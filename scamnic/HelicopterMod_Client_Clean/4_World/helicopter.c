

modded class CarScript
{
	int m_typeVehicle = 0;	// 0 - car; 1 - helicopter; 2 - boat;
	void CarScript()
	{
		RegisterNetSyncVariableInt("m_typeVehicle", 0, 3);
	}


};
modded class DayZPlayerImplement
{
	bool GetCamera3rdPerson()
	{
		return m_Camera3rdPerson;
	}
};


class Heli_sib_cript extends CarScript {};

class HeliTest_Door_1_1 extends CarDoor {};
class HeliTest_Door_1_2 extends CarDoor {};
class HeliTest_Door_2_1 extends CarDoor {};
class HeliTest_Door_2_2 extends CarDoor {};

class HeliBattery : CarBattery {}

class HeliTest_SIB extends Heli_sib_cript
{

	//modding
	float c_insideSoundCoeff = 0.5;
	float c_startEffectCoeff = 1.0;
	float c_effectCoeff = 0.1;
	float c_dustEffectCoeff = 2.5;
	int c_effectDust = 	ParticleList.HELICOPTER_DUST;
	int c_effectSmoke = ParticleList.HATCHBACK_COOLANT_OVERHEATED;
	float c_animSpeed = 0.07;
	string c_startSound = "HelicopterStart_SoundSets";
	string c_stopSound = "HelicopterStop_SoundSets";
	string c_rotorSound = "HelicopterRotor_SoundSets";
	string c_engineSound = "HelicopterEngine_SoundSets";
	string c_warningSound = "HelicopterWarning_SoundSets";
	string c_crashSound = "HelicopterCrash_SoundSets";
	string c_nameRotorH = "rpmspeed";//основной ротор анимация
	string c_nameRotorV = "RotorV";//задний ротор анимация
	float c_camDistance = 10.0;
	float c_fuelDistance = 4.2;
	string c_fuelName = "refill"; //точка для заправки мемори
	string c_engineName = "truba";//точка двигателя мемори
	
	float c_altNoForce = 500.000000;
	float maxFuel = 30.0;
	bool m_enable;	
	Particle m_DustGround = NULL;
	Particle m_SmokeEngine = NULL;
	int m_stateHelicopter = 0;//состояние включение выключение,холостой ход
	bool m_cameraStateType = false;
	bool m_cameraState = false;
	ref SoundObject m_waveObj = NULL;
	ref AbstractWave m_wave = NULL;//звук включения и выключения
	ref SoundObject m_rotorObj = NULL;
	ref AbstractWave m_rotor = NULL;//звук лопастей
	ref SoundObject m_engineObj = NULL;
	ref AbstractWave m_engine = NULL;//звук двигателя
	ref SoundObject m_warningObj = NULL;
	ref AbstractWave m_warning = NULL;//звук критического повреждения
	ref SoundObject m_crashObj = NULL;
	ref AbstractWave m_crash = NULL;// бум бум бум ;)
	float m_waveTime = 0.0;//на какой сек включили звук
	float m_SoundWaveTime = 0.0;//сколько времени уже играет звук
	bool m_disableRotorH = false;//отключить анимацию верхнего ротора
	bool m_disableRotorV = false;//отключить анимацию бокового ротора
	bool m_disableEngine = false;
	bool m_playSoundWarning = false;
	bool m_playSoundCrash = false;

	
	bool m_transportSeat = false;//в транспорте игрок или нет
	bool m_sendToServer = true;// 1 раз отправить на сервер информацию чтоб включить полностью двигатель если двигатель выключен был
	int m_LastTimeRotationStart = 0;
	int m_LastTimeRotationStop = 0;
	float m_AnimRotorSpeed = 0.0;
	float m_AnimRotorH = 0.0;
	float m_AnimRotorV = 0.0;
	//add to modding dlc keyboard
	float m_press_Forward = 0.0;
	float m_press_Back = 0.0;
	float m_press_ForwardMouse = 0.0;
	float m_press_BackMouse = 0.0;
	float m_press_CyclicLeft = 0.0;
	float m_press_CyclicRight = 0.0;
	float m_press_CyclicLeftMouse = 0.0;
	float m_press_CyclicRightMouse = 0.0;
	float m_press_Left = 0.0;
	float m_press_Right = 0.0;
	float m_press_Up = 0.0;
	float m_press_Down = 0.0;
	float m_press_LMenu = 0.0;
	int prohod_sek=0;
	bool kolesoz=true;
	bool progrev=false;
	int shet_warn=0;
	float m_engineVolume=1.0;
	float m_rotorVolume=1.0;
	float m_warningVolume=1.0;
	float m_crashVolume=1.0;
	
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
	
	}
	override void OnUpdate( float dt )
    {
	
	}
	
	override void AfterStoreLoad()
	{
		if(EngineIsOn())
		{
			vector HeliPosition = GetPosition();
			vector contact_pos;
			vector contact_dir;
			int contactComponent;
			DayZPhysics().RaycastRV( HeliPosition, HeliPosition - "0 -700 0",contact_pos,contact_dir,contactComponent,NULL,NULL,NULL,false,false,2,10.0	) 	
			SetPosition(contact_pos);
			EngineStop();
		}
	};
	
	void HeliTest_SIB()
	{
		SetEventMask(EntityEvent.SIMULATE | EntityEvent.CONTACT);
		m_dmgContactCoef = 0.075;
		m_typeVehicle = 1;
		if (MemoryPointExists(c_fuelName) )
		{
			m_fuelPos = GetMemoryPointPos(c_fuelName);
		}
		else
		{
			m_fuelPos = "0 0 0";
		}
		
		RegisterNetSyncVariableBool("m_enable");
		RegisterNetSyncVariableFloat("maxFuel" );
		RegisterNetSyncVariableFloat("c_camDistance" );
		RegisterNetSyncVariableFloat("c_altNoForce" );
		RegisterNetSyncVariableFloat("m_engineVolume" );
		RegisterNetSyncVariableFloat("m_rotorVolume" );
		RegisterNetSyncVariableFloat("m_warningVolume" );
		RegisterNetSyncVariableFloat("m_crashVolume" );
		RegisterNetSyncVariableBool("m_disableRotorH");
		RegisterNetSyncVariableBool("m_disableRotorV");
		RegisterNetSyncVariableBool("m_disableEngine");
		RegisterNetSyncVariableBool("m_playSoundWarning");
		RegisterNetSyncVariableBool("m_playSoundCrash");
	}
	void ~HeliTest_SIB()
	{
		if (m_DustGround)m_DustGround.Stop();  
		if (m_SmokeEngine)m_SmokeEngine.Stop();    
		if (m_wave)m_wave.Stop();    
		if (m_rotor)m_rotor.Stop();    
		if (m_engine)m_engine.Stop();    
		if (m_warning)m_warning.Stop();    
		if (m_crash)m_crash.Stop();    
	}
	float GetCameraHeight()
	{
		return c_camDistance;
	}

	override string GetActionCompNameFuel()
	{
		return c_fuelName;
	}
	
	override float GetActionDistanceFuel()
	{
		return c_fuelDistance;
	}

	void KeyboardPilot(float dt)
	{
		if ( GetGame().IsClient()  && m_enable)
		{
			PlayerBase playerGame = PlayerBase.Cast(GetGame().GetPlayer());
			if(playerGame)
			{	
				HumanCommandVehicle	m_ComandTransport = playerGame.GetCommand_Vehicle();
				if(m_ComandTransport)
				{
					if(m_ComandTransport.GetTransport() == this)
					{
						if(PlayerBase.Cast(CrewMember(0)) == playerGame || CrewSize() > 1 && PlayerBase.Cast(CrewMember(1)) == playerGame)
						{
							m_transportSeat = true;	
							UAInterface interface = playerGame.GetInputInterface();
							if(interface)
							{
								if(m_stateHelicopter == 2)
								{
									m_press_Forward = interface.SyncedValue("UASIBHeliForward");
									m_press_Back = interface.SyncedValue("UASIBHeliBack");
									m_press_CyclicLeft = interface.SyncedValue("UASIBHeliCyclicLeft");
									m_press_CyclicRight = interface.SyncedValue("UASIBHeliCyclicRight");
									m_press_Left = interface.SyncedValue("UASIBHeliLeft");
									m_press_Right = interface.SyncedValue("UASIBHeliRight");
									m_press_Up = interface.SyncedValue("UASIBHeliUp");
									m_press_Down = interface.SyncedValue("UASIBHeliDown");
								}
							}	
						}else{
							m_transportSeat = false;		
						}
					}else{
						m_transportSeat = false;	
					}
				}else{
					m_transportSeat = false;	
				}
			}
		}
	}
	
	void AnimStart(float time)
	{
		if ( GetGame().IsClient()  && m_enable)
		{
			if((!m_disableRotorH || !m_disableRotorV) && !m_disableEngine)
			{
				if (time - m_LastTimeRotationStart >= 1000)
				{
					float m_SoundLength = 0;float bufer_speed=0.001;
					if(m_wave)m_SoundLength = m_wave.GetLength();
					if(m_SoundLength > 0)
					{
						float bufer_vrema=Math.Floor(m_SoundLength / 4);
						float vrema=bufer_speed * bufer_vrema;
						float vrema_ostatok=m_SoundLength-bufer_vrema;
						float mnogitel;
						float mnogitel_ost=(c_animSpeed / vrema_ostatok);
						if(m_AnimRotorSpeed <= vrema)
						{
							mnogitel=bufer_speed;
						}else
						{
							mnogitel=mnogitel_ost;
						}
						m_AnimRotorSpeed = m_AnimRotorSpeed + mnogitel;
						if(m_AnimRotorSpeed > c_animSpeed)m_AnimRotorSpeed = c_animSpeed;
						m_LastTimeRotationStart = time;
					}
				}
				if(!m_disableRotorH)
				{
					m_AnimRotorH = m_AnimRotorH + m_AnimRotorSpeed;
					if(m_AnimRotorH >= 1.0)m_AnimRotorH = m_AnimRotorH - 1.0;
					SetAnimationPhase(c_nameRotorH,m_AnimRotorH);  
				}
				if(!m_disableRotorV)
				{
					m_AnimRotorV = m_AnimRotorV + m_AnimRotorSpeed;
					if(m_AnimRotorV >= 1.0)m_AnimRotorV = m_AnimRotorV - 1.0 ;
					SetAnimationPhase(c_nameRotorV,m_AnimRotorV); 
				}			
			}
		}
	}
	
	void AnimStop(float time)
	{
		if ( GetGame().IsClient()  && m_enable)
		{
			if((!m_disableRotorH || !m_disableRotorV) && !m_disableEngine)
			{
				if (time - m_LastTimeRotationStop >= 1000)
				{
					float m_SoundLength = 0;
					if(m_wave)m_SoundLength = m_wave.GetLength();
					if(m_SoundLength > 0)
					{
						m_AnimRotorSpeed = m_AnimRotorSpeed - (c_animSpeed / m_SoundLength);
						if(m_AnimRotorSpeed < 0)m_AnimRotorSpeed = 0.0;
						m_LastTimeRotationStop = time;
					}else
					{
						if(m_stateHelicopter == 0)
						{
							m_AnimRotorSpeed = m_AnimRotorSpeed - 0.001;
							if(m_AnimRotorSpeed < 0)m_AnimRotorSpeed = 0.0;
						}
					}
				}	
				if(!m_disableRotorH)
				{
					if(m_AnimRotorH >= 1.0)m_AnimRotorH = m_AnimRotorH - 1.0;
					m_AnimRotorH = m_AnimRotorH + m_AnimRotorSpeed;
					SetAnimationPhase(c_nameRotorH,m_AnimRotorH);  
				}
				if(!m_disableRotorV)
				{
					if(m_AnimRotorV >= 1.0)m_AnimRotorV = m_AnimRotorV - 1.0 ;
					m_AnimRotorV = m_AnimRotorV + m_AnimRotorSpeed;
					SetAnimationPhase(c_nameRotorV,m_AnimRotorV);
				}			
			}
		}
	}

	void AnimEngine(float dt)
	{
		if ( GetGame().IsClient()  && m_enable)
		{
			if((!m_disableRotorH || !m_disableRotorV) && !m_disableEngine)
			{
				m_AnimRotorSpeed = 0.069091;
				
				if(!m_disableRotorH)
				{
					if(m_AnimRotorH >= 1.0)m_AnimRotorH = m_AnimRotorH - 1.0;
					m_AnimRotorH = m_AnimRotorH + m_AnimRotorSpeed;
					SetAnimationPhase(c_nameRotorH,m_AnimRotorH);  
				}
				if(!m_disableRotorV)
				{
					if(m_AnimRotorV >= 1.0)m_AnimRotorV = m_AnimRotorV - 1.0 ;
					m_AnimRotorV = m_AnimRotorV + m_AnimRotorSpeed;
					SetAnimationPhase(c_nameRotorV,m_AnimRotorV); 
				}			
			}
		}
	}

	void StartPlayFromPos(bool inside, bool reverse)
	{   
		if ( GetGame().IsClient()  && m_enable)
		{
			string name;
			if (m_stateHelicopter == 1)name = c_startSound;
			if (m_stateHelicopter == 0)name = c_stopSound; 
			float m_waveLength = 0.0;
			float pos = 1.0;
			ref AbstractWave tmp;
			ref SoundObjectBuilder soundObjectBuild;
			ref SoundObject soundObj;
			SoundParams soundParam = new SoundParams(name);
			soundObjectBuild = new SoundObjectBuilder(soundParam);
			if (soundObjectBuild != NULL)
			{
				soundObj = soundObjectBuild.BuildSoundObject();
				if (soundObj != NULL)
				{
					soundObj.SetPosition(GetPosition());
					
					tmp = GetGame().GetSoundScene().Play3D(soundObj, soundObjectBuild); 
					tmp.SetPosition(GetPosition());					
				}
			}
			if (tmp)
			{		
				if (m_wave)
				{
					m_waveLength = m_wave.GetLength();//размер звука
					if(m_waveLength > 0)
					{
						float m_SoundWaveTimeStart = (m_waveTime / 100 * m_waveLength) * 100;// на какой сек был включен звук
						float m_SoundWaveTimeMax = m_SoundWaveTimeStart + m_SoundWaveTime;//учитываем когда был включен звук и сколько он уже воспроизовдиться
						if(m_SoundWaveTimeMax < 0)m_SoundWaveTimeMax = 0;
						if(m_SoundWaveTimeMax >= m_waveLength){m_SoundWaveTimeMax = m_waveLength;}
						if(m_SoundWaveTimeMax > 0)
						{
							pos = (m_SoundWaveTimeMax * 100 / m_waveLength) / 100;// вернуть текущую позицию звука 0-1
						}
					}
					m_wave.SetStartOffset(1.0);//применяем здвиг на выключение звука
					m_wave.Restart();//обновляем звук чтоб применился здвиг
				}
				if(reverse)pos = 1.0 - pos;//реверс звука было включение стало выключение и наоборот
				m_waveTime = pos;//запоминаем когда был включен  звук 0-1
				soundObj.SetKind(WaveKind.WAVEEFFECTEX);
				float vol = 1.0;//базовое значение звука
				if (inside)vol = vol * c_insideSoundCoeff;//внутри звук тише
				tmp.SetVolumeRelative(vol);//меняем грокость
				tmp.SetStartOffset(pos);//применяем здвиг звука
				tmp.Restart();//обновляем звук чтоб применился здвиг
				m_wave = tmp;//перензначаем переменную под новый звук
				m_waveObj = soundObj;
				m_SoundWaveTime = 0;
				
			}
		}
	}
	
	void OnChangeCamEngineRotor(bool cam)
	{
		if(cam)
		{	
			if(m_engine){m_engine.SetVolumeRelative(m_engineVolume / 2);}
			if(m_rotor){m_rotor.SetVolumeRelative(m_rotorVolume / 2);}	
		}
		else
		{
			if(m_engine){m_engine.SetVolumeRelative(m_engineVolume);} 
			if(m_rotor){m_rotor.SetVolumeRelative(m_rotorVolume);}		
		}	
	}
	
	void PlaySoundWarning()
	{
		if (m_playSoundWarning)
		{
			//включаем звук аварии		
			if(!m_warning)
			{
				ref SoundObjectBuilder soundObjectBuildWarning;
				ref SoundObject soundObjWarning;
				SoundParams soundParamWarning = new SoundParams(c_warningSound);
				soundObjectBuildWarning = new SoundObjectBuilder(soundParamWarning);
				if (soundObjectBuildWarning != NULL)
				{
					soundObjWarning = soundObjectBuildWarning.BuildSoundObject();
					if (soundObjWarning != NULL)
					{
						soundObjWarning.SetPosition(GetPosition());
						m_warning = GetGame().GetSoundScene().Play3D(soundObjWarning, soundObjectBuildWarning); 
						if(m_warning)
						{
							m_warning.SetPosition(GetPosition());
							m_warning.SetVolumeRelative(m_warningVolume); 
						}
						m_warningObj = soundObjWarning;						
					}
				}
			}	
		}
		//обновление позиции звука
		vector position = GetPosition();
		if(m_warningObj)m_warningObj.SetPosition(position);
		if(m_warning)m_warning.SetPosition(position);
	}

	void PlaySoundCrash()
	{
		if (m_playSoundCrash)
		{
			//включаем звук краш		
			if(!m_crash)
			{
				ref SoundObjectBuilder soundObjectBuildCrash;
				ref SoundObject soundObjCrash;
				SoundParams soundParamCrash = new SoundParams(c_crashSound);
				soundObjectBuildCrash = new SoundObjectBuilder(soundParamCrash);
				if (soundObjectBuildCrash != NULL)
				{
					soundObjCrash = soundObjectBuildCrash.BuildSoundObject();
					if (soundObjCrash != NULL)
					{
						soundObjCrash.SetPosition(GetPosition());
						m_crash = GetGame().GetSoundScene().Play3D(soundObjCrash, soundObjectBuildCrash); 
						if(m_crash)
						{
							m_crash.SetPosition(GetPosition());
							m_crash.SetVolumeRelative(m_crashVolume); 
						}
						m_crashObj = soundObjCrash;						
					}
				}
			}	
			
		}
		//обновление позиции звука
		vector position = GetPosition();
		if(m_crashObj)m_crashObj.SetPosition(position);
		if(m_crash)m_crash.SetPosition(position);
	}
	
	void StartRotorFromPos(bool cam)
	{
		//включаем звук ротора	
		if(!m_rotor  && !m_disableRotorH && !m_disableEngine)
		{
			ref SoundObjectBuilder soundObjectBuildRotor;
			ref SoundObject soundObjRotor;
			SoundParams soundParamRotor = new SoundParams(c_rotorSound);
			soundObjectBuildRotor = new SoundObjectBuilder(soundParamRotor);
			if (soundObjectBuildRotor != NULL)
			{
				soundObjRotor = soundObjectBuildRotor.BuildSoundObject();
				if (soundObjRotor != NULL)
				{
					soundObjRotor.SetPosition(GetPosition());
					m_rotor = GetGame().GetSoundScene().Play3D(soundObjRotor, soundObjectBuildRotor);  
					if(m_rotor)
					{
						m_rotor.SetPosition(GetPosition());
						m_rotor.Loop(true);
						m_rotor.SetVolumeRelative(m_rotorVolume); //0.0
					}
					m_rotorObj = soundObjRotor;			
				}
			}
		}
		
		
		
		//обновление позиции звука ротора и двигателя
		vector position = GetPosition();
		if(m_rotorObj)m_rotorObj.SetPosition(position);
		if(m_rotor)m_rotor.SetPosition(position);
	}
	
	void StartEngineFromPos(bool cam)
	{
		
		//включаем звук двигателя		
		if(!m_engine && !m_disableEngine)
		{
			ref SoundObjectBuilder soundObjectBuildEngine;
			ref SoundObject soundObjEngine;
			SoundParams soundParamEngine = new SoundParams(c_engineSound);
			soundObjectBuildEngine = new SoundObjectBuilder(soundParamEngine);
			if (soundObjectBuildEngine != NULL)
			{
				soundObjEngine = soundObjectBuildEngine.BuildSoundObject();
				if (soundObjEngine != NULL)
				{
					soundObjEngine.SetPosition(GetPosition());
					m_engine = GetGame().GetSoundScene().Play3D(soundObjEngine, soundObjectBuildEngine); 
					if(m_engine)
					{
						m_engine.SetPosition(GetPosition());
						m_engine.Loop(true);				
						m_engine.SetVolumeRelative(m_engineVolume); //0.0
					}
					m_engineObj = soundObjEngine;						
				}
			}
		}
		
		//обновление позиции звука ротора и двигателя
		vector position = GetPosition();
		if(m_engineObj)m_engineObj.SetPosition(position);
		if(m_engine)m_engine.SetPosition(position);
	}
	
	bool IsSurfaceWater( vector position )
	{
		CGame game = GetGame();
		return game.SurfaceIsSea( position[0], position[2] ) || game.SurfaceIsPond( position[0], position[2] );
	}
	
	void EffectDust(float pos_sound,vector position,int health_level)
	{
		if(!IsSurfaceWater(position))
		{
			vector groundPos = position;
			groundPos[1] = GetGame().SurfaceRoadY(groundPos[0], groundPos[2]);
			float distance = vector.Distance(position, groundPos);	
			if(pos_sound > 0.0 && health_level < 4 && !m_disableRotorH && !m_disableEngine && distance < 3)
			{
				if (!m_DustGround)m_DustGround = Particle.CreateInWorld(c_effectDust, groundPos );
				if (m_DustGround )
				{
					m_DustGround.PlayParticle();	
					m_DustGround.SetPosition(groundPos);
					m_DustGround.SetParameter(-1,EmitorParam.SIZE, pos_sound * c_dustEffectCoeff);
				}
			}
			else
			{
				if (m_DustGround)m_DustGround.Stop();
			}
		}
		else
		{
			if (m_DustGround)m_DustGround.Stop();		
		}	
	};
	
	
	void EffectSmokeEngine(float pos_sound,int health_level)
	{

		float type = 0;
		if(pos_sound > 0.0 && pos_sound < 0.4 && m_stateHelicopter == 1 && health_level < 4 && type == 0)
		{type = c_startEffectCoeff;}//включение двигателя
		else if(pos_sound > 0.0 && health_level > 0 && type == 0)
		{type = health_level + c_effectCoeff * 4;}//поврежден верт
		else if(pos_sound > 0.4 && health_level < 4 && type == 0)
		{type = c_effectCoeff;}//просто дым с двигателя
				
		
		
		
		if(pos_sound > 0.4 && health_level < 4){type = c_effectCoeff;}
		
		if(type > 0 && !m_disableEngine)
		{
			vector m_engineDmgPos = "0 0 0";	
			if ( MemoryPointExists(c_engineName))m_engineDmgPos = GetMemoryPointPos(c_engineName);
			vector ref_pos = ModelToWorld(m_engineDmgPos);
			if (!m_SmokeEngine){m_SmokeEngine = Particle.CreateInWorld(c_effectSmoke, ref_pos);}
			if (m_SmokeEngine)
			{
				m_SmokeEngine.PlayParticle();	
				m_SmokeEngine.SetPosition(ref_pos);
				m_SmokeEngine.SetParameter(-1,EmitorParam.SIZE, type);
			}
		}
		else
		{
			if (m_SmokeEngine)
			{
				m_SmokeEngine.Stop();
			}
		}
		
	}
	
	void SoundAnim_Simulate(float dt)
	{
		if ( GetGame().IsClient()  && m_enable)
		{
			vector helicopterPos = GetPosition();
			int time = GetGame().GetTime();
			int health_level = GetHealthLevel("Engine");
			PlayerBase playerGame = PlayerBase.Cast(GetGame().GetPlayer());
			bool m_stateHelicopterType = m_stateHelicopter;
			float anim_engine = GetAnimationPhase("hide_start");
			float anim_rotor =  GetAnimationPhase("hide_rotor");
			if(anim_engine == 0.0 && anim_rotor == 0.0){m_stateHelicopter = 0;m_sendToServer = true;}
			if(anim_engine == 1.0 && anim_rotor == 0.0){m_stateHelicopter = 1;}
			if(anim_engine == 1.0 && anim_rotor == 1.0){m_stateHelicopter = 2;m_sendToServer = true;}
			if(m_stateHelicopter == 0){AnimStop(time);}
			if(m_stateHelicopter == 1){AnimStart(time);}
			if(m_stateHelicopter == 2){AnimEngine(time);}
			
			
			if(m_waveObj)m_waveObj.SetPosition(helicopterPos);
			if(m_wave)m_wave.SetPosition(helicopterPos);


			if(playerGame)
			{

				m_cameraStateType = playerGame.IsCameraInsideVehicle();
				//смена звука при смене камеры (звук включения и выключения двигателя)	
				if(m_wave && m_cameraStateType != m_cameraState  && (m_stateHelicopter == 0 || m_stateHelicopter == 1))
				{
					m_cameraState = m_cameraStateType;
					StartPlayFromPos(m_cameraStateType, false);
				}
				
				//включения и выключение двигателя проверка камеры
				if(m_stateHelicopterType != m_stateHelicopter && (m_stateHelicopter == 0 || m_stateHelicopter == 1))
				{
					StartPlayFromPos(m_cameraStateType, true);
				}
				
				//изменение громкости звука двигателя и ротора при смене камеры	
				if(m_cameraStateType != m_cameraState  && m_stateHelicopter == 2)
				{
					m_cameraState = m_cameraStateType;
					OnChangeCamEngineRotor(m_cameraStateType);
				}
			}

			
			float soundPlayTime;
			float soundTime;
			float pos = 0.0;
			if(m_wave && (m_stateHelicopter == 0 || m_stateHelicopter == 1))
			{	
				float m_waveLength = m_wave.GetLength();			
				soundPlayTime = (m_waveTime / 100 * m_waveLength) * 100;	
				soundTime = soundPlayTime + m_SoundWaveTime;	
				pos = (soundTime * 100 / m_waveLength) / 100;
				if(m_stateHelicopter == 0)pos = 1.0 - pos;
				pos = Math.Clamp(pos, 0, 1);
				
				
				if(m_cameraStateType)
				{
					if(m_engine){m_engine.SetVolumeRelative(pos * (m_engineVolume/ 4));}
					if(m_rotor){m_rotor.SetVolumeRelative(pos * (m_rotorVolume/ 4));}
				}
				else
				{
					if(m_engine){m_engine.SetVolumeRelative(pos * (m_engineVolume/ 2));}
					if(m_rotor){m_rotor.SetVolumeRelative(pos * (m_rotorVolume / 2));}
				}

				
				if(m_sendToServer && m_stateHelicopter == 1)
				{			
					if(playerGame && m_transportSeat)
					{	
						if((soundTime + 1.5) >= m_waveLength)
						{
							ref array<ref Param> paramsStartSimul = new array<ref Param>;
							Param1<string> paramNameFunction = new Param1<string>("HelicopterStartSimulate");
							paramsStartSimul.Insert(paramNameFunction);
							Param2<EntityAI,EntityAI> paramInfo = new Param2<EntityAI,EntityAI>(this,playerGame);
							paramsStartSimul.Insert(paramInfo);
							GetRPCManager_SMOD().SendRPC(paramsStartSimul, false, NULL);//с клиента на сервер
							m_sendToServer = false;
						}	
					}
				}
				
			}
			else
			{
				if(m_stateHelicopter == 2)pos = 1.0;	
				if(m_stateHelicopter == 0 || m_stateHelicopter == 1)
				{
					if(m_rotor)
					{	
						m_rotor.Loop(false);
						m_rotor.SetVolumeRelative(0.0); 	
						m_rotor.SetStartOffset(1.0);
						m_rotor.Restart();
					}

					if(m_engine)
					{
						m_engine.Loop(false);
						m_engine.SetVolumeRelative(0.0); 
						m_engine.SetStartOffset(1.0);
						m_engine.Restart();
					}		
				}
			}
			if(m_stateHelicopter == 2|| m_stateHelicopter == 1 && pos > 0.7 || m_stateHelicopter == 0 && pos > 0.7)
			{
				StartRotorFromPos(m_cameraStateType);
			}
			if( m_stateHelicopter == 2 || m_stateHelicopter == 1 && pos > 0.7)
			{
				StartEngineFromPos(m_cameraStateType);
			}
			  
				if(m_stateHelicopter == 0  && pos < 0.7)
				{
					if(m_rotor)
					{	
						m_rotor.Loop(false);
						m_rotor.SetVolumeRelative(0.0); 	
						m_rotor.SetStartOffset(1.0);
						m_rotor.Restart();
					}

					if(m_engine)
					{
						m_engine.Loop(false);
						m_engine.SetVolumeRelative(0.0); 
						m_engine.SetStartOffset(1.0);
						m_engine.Restart();
					}		
				}
			  
			if (m_wave)
			{
				if(m_disableEngine)
				{
					m_wave.SetVolumeRelative(0.0); 	
				}
				m_SoundWaveTime += dt;
			}
			else
			{
				m_SoundWaveTime = 0.0;
			}
			
			
			//отключаем звук ротора если ротор уничтожен ли поврежден
			if(m_disableRotorH)
			{
				if(m_rotor)
				{
					m_rotor.Loop(false);
					m_rotor.SetVolumeRelative(0.0); 	
					m_rotor.SetStartOffset(1.0);
					m_rotor.Restart();
				}
			}
			
			
			
			//отключаем звук мотора и звук лопастей если двигатель уничтожен
			if(m_disableEngine)
			{	
				if(m_rotor)
				{
					m_rotor.Loop(false);
					m_rotor.SetVolumeRelative(0.0); 	
					m_rotor.SetStartOffset(1.0);
					m_rotor.Restart();
				}
				if(m_engine)
				{
					m_engine.Loop(false);
					m_engine.SetVolumeRelative(0.0); 
					m_engine.SetStartOffset(1.0);
					m_engine.Restart();
				}
			}
			
			
			PlaySoundWarning();	
			PlaySoundCrash();
			EffectDust(pos,helicopterPos,health_level);
			EffectSmokeEngine(pos,health_level);
			
		}
	}
	
	void creat_InInventory_heli0()
	{
		GetInventory().CreateInInventory( "HatchbackWheel" );
		GetInventory().CreateInInventory( "HatchbackWheel" );
		GetInventory().CreateInInventory( "HatchbackWheel" );
		GetInventory().CreateInInventory( "HatchbackWheel" );
	};
	void creat_InInventory_heli()
	{
		if(IsKindOf("HeliTest_SIB1"))
		{
			GetInventory().CreateInInventory( "HeliTest_Door_1_1" );
			GetInventory().CreateInInventory( "HeliTest_Door_1_2" );
			GetInventory().CreateInInventory( "HeliTest_Door_2_1" );
			GetInventory().CreateInInventory( "HeliTest_Door_2_2" );
		}
		if(IsKindOf("HeliTest_SIB0"))
		{
			GetInventory().CreateInInventory( "HeliTest_Door_1_1_black" );
			GetInventory().CreateInInventory( "HeliTest_Door_1_2_black" );
			GetInventory().CreateInInventory( "HeliTest_Door_2_1_black" );
			GetInventory().CreateInInventory( "HeliTest_Door_2_2_black" );
		}
		if(IsKindOf("HeliTest_SIB3"))
		{
			GetInventory().CreateInInventory( "HeliTest_Door_1_1_white" );
			GetInventory().CreateInInventory( "HeliTest_Door_1_2_white" );
			GetInventory().CreateInInventory( "HeliTest_Door_2_1_white" );
			GetInventory().CreateInInventory( "HeliTest_Door_2_2_white" );
		}
	};
	override void EOnSimulate( IEntity owner, float dt)
	{
		if ( GetGame().IsClient()  && m_enable)
		{

			SoundAnim_Simulate(dt);
			KeyboardPilot(dt);
			
		}

		if ( GetGame().IsServer() )
		{
			if(prohod_sek < 5)
			{
				prohod_sek=prohod_sek + 1;
				if(prohod_sek == 4 && kolesoz)
				{
					creat_InInventory_heli0();
					creat_InInventory_heli();
				}
			}
		}
	}


	override int GetAnimInstance()
	{
		return VehicleAnimInstances.S120;
	}


	override int GetSeatAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 2:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 3:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		}
		return 0;
	}
	
	
	// Override for car-specific light type
	override CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast( ScriptedLightBase.CreateLight(Sedan_02FrontLight) );
	}
	
	
	// Override for car-specific light type
	override CarRearLightBase CreateRearLight()
	{
		return CarRearLightBase.Cast( ScriptedLightBase.CreateLight(Sedan_02RearLight) );
	}
	
	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		return false;
	}


	override bool IsVitalTruckBattery()
	{
		return false;
	}


	override bool IsVitalGlowPlug()
	{
		return false;
	}


	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "HeliTest_Door_1_1";
			break;
		case 1:
			return "HeliTest_Door_2_1";
			break;
		case 2:
			return "HeliTest_Door_1_2";
			break;
		case 3:
			return "HeliTest_Door_2_2";
			break;
		}
		
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}
	


	override int GetCarDoorsState( string slotType )
	{
		CarDoor carDoor;

		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );
		if ( !carDoor )return CarDoorState.DOORS_MISSING;
		
		switch( slotType )
		{
		case "HeliTest_Door_1_1":
			if ( GetAnimationPhase("DoorsDriver") > 0.5 )
			return CarDoorState.DOORS_OPEN;
			else
			return CarDoorState.DOORS_CLOSED;

			break;
			
		case "HeliTest_Door_2_1":
			if ( GetAnimationPhase("DoorsCoDriver") > 0.5 )
			return CarDoorState.DOORS_OPEN;
			else
			return CarDoorState.DOORS_CLOSED;

			break;
			
		case "HeliTest_Door_1_2":
			if ( GetAnimationPhase("DoorsCargo1") > 0.5 )
			return CarDoorState.DOORS_OPEN;
			else
			return CarDoorState.DOORS_CLOSED;

			break;
			
		case "HeliTest_Door_2_2":
			if ( GetAnimationPhase("DoorsCargo2") > 0.5 )
			return CarDoorState.DOORS_OPEN;
			else
			return CarDoorState.DOORS_CLOSED;
			break;
			
			
			
		}

		return CarDoorState.DOORS_MISSING;
	}
	
	

	override string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "doors_driver";
			break;
		case 1:
			return "doors_codriver";
			break;
		case 2:
			return "doors_cargo1";
			break;
		case 3:
			return "doors_cargo2";
			break;
		}
		
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}
	
	

	override string GetAnimSourceFromSelection( string selection )
	{
		switch( selection )
		{
		case "doors_driver":
			return "DoorsDriver";
		case "doors_codriver":
			return "DoorsCoDriver";
		case "doors_cargo1":
			return "DoorsCargo1";
		case "doors_cargo2":
			return "DoorsCargo2";
		}

		return "";
	}
	
	

	override bool CrewCanGetThrough( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			if ( GetCarDoorsState( "HeliTest_Door_1_1" ) == CarDoorState.DOORS_CLOSED )
			return false;

			return true;
			break;
			
		case 1:
			if ( GetCarDoorsState( "HeliTest_Door_2_1" ) == CarDoorState.DOORS_CLOSED )
			return false;

			return true;
			break;

		case 2:
			if ( GetCarDoorsState( "HeliTest_Door_1_2" ) == CarDoorState.DOORS_CLOSED )
			return false;

			return true;
			break;

		case 3:
			if ( GetCarDoorsState( "HeliTest_Door_2_2" ) == CarDoorState.DOORS_CLOSED )
			return false;

			return true;
			break;
		}

		return false;
	}
	
	

	override bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
		switch( currentSeat )
		{
		case 0:
			if ( nextSeat == 1 )
			return true;
			break;
		case 1:
			if ( nextSeat == 0 )
			return true;
			break;
		case 2:
			if ( nextSeat == 3 )
			return true;
			break;
		case 3:
			if ( nextSeat == 2 )
			return true;
			break;
		}
		
		return false;
	}
	

	override bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
		switch( pCurrentSeat )
		{
		case 0:
			if (pDoorsSelection == "DoorsDriver")
			{
				return true;
			}
			break;
		case 1:
			if (pDoorsSelection == "DoorsCoDriver")
			{
				return true;
			}
			break;
		case 2:
			if (pDoorsSelection == "DoorsCargo1")
			{
				return true;
			}
			break;
		case 3:
			if (pDoorsSelection == "DoorsCargo2")
			{
				return true;
			}
			break;
		}
		
		return false;		
	}



	override bool CanDisplayAttachmentCategory( string category_name )
	{
		return false;
	}
	void falsekolesoz(string slot_name)
	{
		if ( GetGame().IsServer() )
		{
			if(slot_name=="NivaWheel_1_1")
			{
				kolesoz=false;
			}
		}
	}
	override void EEItemAttached ( EntityAI item, string slot_name ) 
	{
		//super.EEItemAttached( item, slot_name );
		falsekolesoz(slot_name);
	}

};


class HeliSIB_big extends HeliTest_SIB{}
class HeliSIB_middle extends HeliTest_SIB{}
class HeliSIB_little extends HeliTest_SIB{}
class HeliTest_SIB_base extends HeliTest_SIB{}



ref map<string,float> ssaBWDQnlAkkAQp = new map<string,float>;
class JsonFileLoaderVert<Class T>
{
	protected static ref JsonSerializer m_Serializer = new JsonSerializer;

	static void JsonLoadFile( string filename, out T data )
	{
		if( FileExist( filename ) )
		{
			string file_content;
			string line_content;
			string error;

			FileHandle handle = OpenFile( filename, FileMode.READ );
			if ( handle == 0 )
			return;

			while ( FGets( handle,  line_content ) >= 0 )
			{
				file_content += line_content;
			}

			CloseFile( handle );

			if( !m_Serializer )
			m_Serializer = new JsonSerializer;

			if( !m_Serializer.ReadFromString( data, file_content, error ) )
			{
				Error( error );
			}
		}

	}

	static void JsonSaveFile( string filename, T data )
	{
		string file_content;
		string file_content_result;

		if( !m_Serializer )
		m_Serializer = new JsonSerializer;

		m_Serializer.WriteToString( data, true, file_content );

		FileHandle handle = OpenFile( filename, FileMode.WRITE );
		if ( handle == 0 )
		return;

		FPrint( handle, file_content ); 
		CloseFile( handle );


	}

	static void JsonLoadData( string string_data, out T data )
	{
		string error;
		if( !m_Serializer )
		m_Serializer = new JsonSerializer;

		if( !m_Serializer.ReadFromString( data, string_data, error ) )
		Error( error );
	}

	static string JsonMakeData( T data )
	{
		string string_data;
		if( !m_Serializer )
		m_Serializer = new JsonSerializer;

		m_Serializer.WriteToString( data, true, string_data );
		return string_data;
	}
	static string JsonMakeData2( T data )
	{
		string string_data;
		if( !m_Serializer )
		m_Serializer = new JsonSerializer;

		m_Serializer.WriteToString( data, false, string_data );
		return string_data;
	}
}






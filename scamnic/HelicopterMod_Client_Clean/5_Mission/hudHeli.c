
modded class IngameHud
{
	Widget      m_HelicopterPanel;
	//скорость
	ImageWidget	m_HelicopterSpeedPointer;
	TextWidget	m_HelicopterSpeedValue;
	//высота	
	ImageWidget	m_HelicopterAltitudePointer;
	TextWidget	m_HelicopterAltitudeValue;
	//горизонт
	ImageWidget	m_HelicopterHorizonPointer;
	ImageWidget	m_HelicopterHorizonDial;
	TextWidget	m_HelicopterHorizonValue;
	//обороты
	ImageWidget	m_HelicopterStabilityPointer;
	TextWidget	m_HelicopterStabilityValue;
	//топливо
	ImageWidget	m_HelicopterFuelPointer;
	TextWidget	m_HelicopterFuelValue;

	override void Init( Widget hud_panel_widget )
	{
		super.Init( hud_panel_widget);	
		
		WrapSpacerWidget m_AllWidget = WrapSpacerWidget.Cast( m_HudPanelWidget.FindAnyWidget( "LeftHUDPanel" ) );
		Widget widget 	= GetGame().GetWorkspace().CreateWidgets( "HelicopterModScripts/GUI/helicopter.layout", m_AllWidget);

		
		m_HelicopterPanel = m_HudPanelWidget.FindAnyWidget("HudFrameWidget");
		m_HelicopterPanel.Show( false );	
		m_HelicopterSpeedPointer			= ImageWidget.Cast( m_HelicopterPanel.FindAnyWidget("HelicopterSpeedPointer") );
		m_HelicopterSpeedValue				= TextWidget.Cast( m_HelicopterPanel.FindAnyWidget("HelicopterSpeedText") );
		
		m_HelicopterAltitudePointer			= ImageWidget.Cast( m_HelicopterPanel.FindAnyWidget("HelicopterAltitudePointer") );
		m_HelicopterAltitudeValue				= TextWidget.Cast( m_HelicopterPanel.FindAnyWidget("HelicopterAltitudeText") );
		
		
		
		m_HelicopterFuelPointer			= ImageWidget.Cast( m_HelicopterPanel.FindAnyWidget("HelicopterFuelPointer") );
		

	}

	override void RefreshVehicleHud( float timeslice )
	{
		super.RefreshVehicleHud(timeslice);	
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( player )
		{
			HumanCommandVehicle hcv = player.GetCommand_Vehicle();	
			if ( hcv )
			{			
				HeliTest_SIB helicoter = HeliTest_SIB.Cast( hcv.GetTransport() );
				if( helicoter )
				{
					
						PlayerBase driverHelicopter = PlayerBase.Cast(helicoter.CrewMember(0));
						PlayerBase codriverHelicopter;
						if(helicoter.CrewSize() > 1)
						{
							codriverHelicopter = PlayerBase.Cast(helicoter.CrewMember(1));
						}
						helicoter.SetAnimationPhase( "HorizonDivedetect", helicoter.GetDirection()[1]);// горизонт
						helicoter.SetAnimationPhase( "HorizonBankdetect", helicoter.GetOrientation()[2] / 360);// горизонт
						helicoter.SetAnimationPhase( "compassdetect", helicoter.GetOrientation()[0] * Math.DEG2RAD );// компас
						helicoter.SetAnimationPhase( "rpmdetect", helicoter.m_AnimRotorSpeed * 100 );//обороты
						float toplivo = helicoter.GetAnimationPhase("toplivo");
						float toplivo_value = ( toplivo * 100 );
						float toplivoResult = ( toplivo_value / helicoter.maxFuel );
						helicoter.SetAnimationPhase( "toplivodetect", toplivoResult ); // топливо
						vector posUp = helicoter.GetPosition();
						vector posDown = helicoter.GetPosition();
						posDown[1] = GetGame().SurfaceY(posDown[0], posDown[2]);
						float m_height = vector.Distance( posUp, posDown );
						float height_value = ( m_height / helicoter.c_altNoForce );
						helicoter.SetAnimationPhase( "visotadetect", height_value );//высота
						helicoter.SetAnimationPhase( "visota_num_detect", m_height );//высота2
						float m_height_fix;
						if(m_height<100)
						{
							m_height_fix=m_height /8.3;
						}else
						{
							m_height_fix=m_height /50 + 9;
						}
						helicoter.SetAnimationPhase( "visota_num_detect2", m_height_fix );//высота 3
						if(driverHelicopter && driverHelicopter.GetCamera3rdPerson() && driverHelicopter && driverHelicopter == player || codriverHelicopter && codriverHelicopter.GetCamera3rdPerson() && codriverHelicopter && codriverHelicopter== player)
						{
							float speedX_value = ( helicoter.GetSpeedometer() / 300 );
							m_HelicopterSpeedPointer.SetRotation( 0, 0, speedX_value * 351, true );
							m_HelicopterSpeedValue.SetText( Math.Floor( helicoter.GetSpeedometer() ).ToString() );	

							m_HelicopterAltitudePointer.SetRotation( 0, 0, height_value * 351, true );
							m_HelicopterAltitudeValue.SetText( Math.Floor(m_height).ToString() );	
							
							m_HelicopterFuelPointer.SetRotation( 0, 0, toplivoResult * 260 - 130, true );
							
							m_HelicopterPanel.Show( true );
							//GetUApi().GetInputByName( "UACarShiftGearUp" ).ForceDisable( true );
							//GetUApi().GetInputByName( "UACarShiftGearDown" ).ForceDisable( true );
						}
						else
						{
							m_HelicopterPanel.Show( false );	
						}
					m_VehiclePanel.Show( false );
				}else{
					m_HelicopterPanel.Show( false );	
				//GetUApi().GetInputByName( "UACarShiftGearUp" ).ForceDisable( false );
				//GetUApi().GetInputByName( "UACarShiftGearDown" ).ForceDisable( false );
				}
			}else{
				m_HelicopterPanel.Show( false );	
			}
		}
	}	
};

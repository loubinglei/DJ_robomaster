#include "main.h"

/************************** Variable Definitions *****************************/
PID GMPPositionPID = GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT;
PID GMPSpeedPID = GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT;
PID GMYPositionPID = GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT;
PID GMYSpeedPID = GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT;

PID CMRotatePID = CHASSIS_MOTOR_ROTATE_PID_DEFAULT;
PID CM1SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID CM2SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID CM3SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID CM4SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;

//PID_Regulator_t ShootMotorPositionPID = SHOOT_MOTOR_POSITION_PID_DEFAULT;      //shoot motor
//PID_Regulator_t ShootMotorSpeedPID = SHOOT_MOTOR_SPEED_PID_DEFAULT;

/*--------------------------------------------CTRL Variables----------------------------------------*/
WorkState_e lastWorkState = PREPARE_STATE;
WorkState_e workState = PREPARE_STATE;
//RampGen_t GMPitchRamp = RAMP_GEN_DAFAULT;
//RampGen_t GMYawRamp = RAMP_GEN_DAFAULT;
	
/*
*********************************************************************************************************
*                                            FUNCTIONS 
*********************************************************************************************************
*/

static void SetWorkState(WorkState_e state)
{
    workState = state;
}

WorkState_e GetWorkState()
{
	return workState;
}
//底盘控制任务
void CMControlLoop(void)
{  
	//底盘旋转量计算
	if(GetWorkState()==PREPARE_STATE) //启动阶段，底盘不旋转
	{
		ChassisSpeedRef.rotate_ref = 0;	 
	}
	else
	{
		 //底盘跟随编码器旋转PID计算
		 CMRotatePID.Ref = 0;
		 CMRotatePID.Fdb = GMYawEncoder.ecd_angle;
		 CMRotatePID.calc(&CMRotatePID);
		 ChassisSpeedRef.rotate_ref = CMRotatePID.Out;
	}
	if((GetWorkState() == STOP_STATE))      //如果遥控器丢失，强制将速度设定值reset
	{
		ChassisSpeedRef.forward_back_ref = 0;
		ChassisSpeedRef.left_right_ref = 0;
	}


	//CMxSpeedPID.Ref是遥控器发来的数据 作为reference，相当于目标值，与下面的Fdb（实际值）作对比，得到偏差，作为PID的输入
	CM1SpeedPID.Ref = 53.872*(-ChassisSpeedRef.forward_back_ref*0.075 + ChassisSpeedRef.left_right_ref*0.075);// + ChassisSpeedRef.rotate_ref;
	CM2SpeedPID.Ref = 53.872*(ChassisSpeedRef.forward_back_ref*0.075 + ChassisSpeedRef.left_right_ref*0.075);// + ChassisSpeedRef.rotate_ref;
	CM3SpeedPID.Ref = 53.872*(ChassisSpeedRef.forward_back_ref*0.075 - ChassisSpeedRef.left_right_ref*0.075);// + ChassisSpeedRef.rotate_ref;
	CM4SpeedPID.Ref = 53.872*(-ChassisSpeedRef.forward_back_ref*0.075 - ChassisSpeedRef.left_right_ref*0.075);// + ChassisSpeedRef.rotate_ref;

	//CMxSpeedPID.Fdb是通过CAN_1接收到的编码器数据，作为速度反馈
	CM1SpeedPID.Fdb = (float)CM1Encoder.filter_rate;
	CM2SpeedPID.Fdb = (float)CM2Encoder.filter_rate;
	CM3SpeedPID.Fdb = (float)CM3Encoder.filter_rate;
	CM4SpeedPID.Fdb = (float)CM4Encoder.filter_rate;
	

	CM1SpeedPID.calc(&CM1SpeedPID);
	CM2SpeedPID.calc(&CM2SpeedPID);
	CM3SpeedPID.calc(&CM3SpeedPID);
	CM4SpeedPID.calc(&CM4SpeedPID);
	
	//CM1SpeedPID.Out最大为5000，这个值是按照以前DJ工程设定的，不会有错   我试过直接赋值Set_CM_Speed(CanInstPtr_1,300,300,300,300)车子缓慢，说明5000的最大值是靠谱的
	 if((GetWorkState() == STOP_STATE) || GetWorkState() == CALI_STATE || GetWorkState() == PREPARE_STATE)
	 {
		 Set_CM_Speed(CanInstPtr_1, 0,0,0,0);
	 }
	 else
	 {
		 Set_CM_Speed(CanInstPtr_1, CHASSIS_SPEED_ATTENUATION * CM1SpeedPID.Out, CHASSIS_SPEED_ATTENUATION * CM2SpeedPID.Out, CHASSIS_SPEED_ATTENUATION * CM3SpeedPID.Out, CHASSIS_SPEED_ATTENUATION * CM4SpeedPID.Out);
	 }
	//Set_CM_Speed(CanInstPtr_1,4950,4950,4950,4950);
}







//发射机构射击电机任务
int16_t pwm_ccr = 0;
void ShooterMControlLoop(void)	
{				      
/*	if(GetShootState() == SHOOTING)
	{
		ShootMotorSpeedPID.ref = PID_SHOOT_MOTOR_SPEED;				
	}
	else
	{
		ShootMotorSpeedPID.ref = 0;		
	}
	
	ShootMotorSpeedPID.fdb = GetQuadEncoderDiff();   
	ShootMotorSpeedPID.Calc(&ShootMotorSpeedPID);
	PWM3 = ShootMotorSpeedPID.output;	
*/
}


/**********************************************************
*工作状态切换状态机,与1ms定时中断同频率
**********************************************************/

void WorkStateFSM(int time_tick_1ms)
{
	lastWorkState = workState;
	switch(workState)
	{
		case PREPARE_STATE:
		{
			if(GetInputMode() == STOP)
			{
				workState = STOP_STATE;
			}
			else if(time_tick_1ms > PREPARE_TIME_TICK_MS)
			{
				workState = NORMAL_STATE;
			}			
		}break;
		case NORMAL_STATE:     
		{
			if(GetInputMode() == STOP)
			{
				workState = STOP_STATE;
			}			
		}break;
		case STANDBY_STATE:     
		{
			if(GetInputMode() == STOP)
			{
				workState = STOP_STATE;
			}				
		}break;
		case STOP_STATE:   
		{
			if(GetInputMode() != STOP)
			{
				workState = PREPARE_STATE;   
			}
		}break;
		case CALI_STATE:      
		{
			if(GetInputMode() == STOP)
			{
				workState = STOP_STATE;
			}
		}break;	    
		default:
		{
			
		}
	}	
}


void WorkStateSwitchProcess(void)
{
	//如果从其他模式切换到prapare模式，要将一系列参数初始化
	if((lastWorkState != workState) && (workState == PREPARE_STATE))  
	{
		ControtLoopTaskInit();
		RemoteTaskInit();
	}
}
/*
************************************************************************************************************************
*Name        : GimbalYawControlModeSwitch
* Description: This function process the yaw angle ref and fdb according to the WORKSTATE.
* Arguments  : void     
* Returns    : void
* Note(s)    : 1) from NORMAL to STANDBY it need to delay a few seconds to wait for the IMU to be steady.  
                  STATE_SWITCH_DELAY_TICK represents the delay time.
************************************************************************************************************************
*/

void GimbalYawControlModeSwitch(void)
{
	static uint8_t normalFlag = 0;   //正常工作模式标志
	static uint8_t standbyFlag = 1;  //IMU工作模式标志
	static uint32_t modeChangeDelayCnt = 0;
	static float angleSave = 0.0f;    //用于切换模式时保存切换前的角度值，用于角度给定值切换
	switch(GetWorkState())
	{
		case PREPARE_STATE:   //启动过程，加入斜坡
		{
			GMYPositionPID.Ref = 0.0f;
			GMYPositionPID.Fdb = -GMYawEncoder.ecd_angle;//*GMYawRamp.Calc(&GMYawRamp);
			angleSave = ZGyroModuleAngle;			
		}break;
		case NORMAL_STATE:
		{
			if(standbyFlag == 1)
			{
				standbyFlag = 0;
				normalFlag = 1;
				GimbalRef.yaw_angle_dynamic_ref = angleSave;   //修改设定值为STANDBY状态下记录的最后一个ZGYROMODULEAngle值
				modeChangeDelayCnt = 0;   //delay清零
			}
			GMYPositionPID.Ref = GimbalRef.yaw_angle_dynamic_ref;   //设定给定值
			GMYPositionPID.Fdb = ZGyroModuleAngle; 					//设定反馈值
			angleSave = yaw_angle;   //时刻保存IMU的值用于从NORMAL向STANDBY模式切换
		}break;
		case STANDBY_STATE:   //IMU模式
		{
			modeChangeDelayCnt++;
			if(modeChangeDelayCnt < STATE_SWITCH_DELAY_TICK)    //delay的这段时间与NORMAL_STATE一样
			{
				GMYPositionPID.Ref = GimbalRef.yaw_angle_dynamic_ref;   //设定给定值
				GMYPositionPID.Fdb = ZGyroModuleAngle; 					//设定反馈值
				angleSave = yaw_angle;
			}
			else     //delay时间到，切换模式到IMU
			{
				if(normalFlag == 1)   //修改模式标志
				{
					normalFlag = 0;
					standbyFlag = 1;
					GimbalRef.yaw_angle_dynamic_ref = angleSave;    //保存的是delay时间段内保存的
				}
				GMYPositionPID.Ref = GimbalRef.yaw_angle_dynamic_ref;   //设定给定值
				GMYPositionPID.Fdb = yaw_angle; 					//设定反馈值
				angleSave = ZGyroModuleAngle;           //IMU模式时，保存ZGyro的值供模式切换时修改给定值使用						
			}
		}break;
		case STOP_STATE:    //停止工作模式
		{
			
		}break;
		case CALI_STATE:    //校准模式
		{
			
		}break;
	}	
}

//云台pitch轴控制程序
void GMPitchControlLoop(void)
{
	GMPPositionPID.Kp = PITCH_POSITION_KP_DEFAULTS + PitchPositionSavedPID.kp_offset;
	GMPPositionPID.Ki = PITCH_POSITION_KI_DEFAULTS + PitchPositionSavedPID.ki_offset;
	GMPPositionPID.Kd = PITCH_POSITION_KD_DEFAULTS + PitchPositionSavedPID.kd_offset;
		
	GMPSpeedPID.Kp = PITCH_SPEED_KP_DEFAULTS + PitchSpeedSavedPID.kp_offset;
	GMPSpeedPID.Ki = PITCH_SPEED_KI_DEFAULTS + PitchSpeedSavedPID.ki_offset;
	GMPSpeedPID.Kd = PITCH_SPEED_KD_DEFAULTS + PitchSpeedSavedPID.kd_offset;
	
	GMPPositionPID.Ref = GimbalRef.pitch_angle_dynamic_ref;
	GMPPositionPID.Fdb = -GMPitchEncoder.ecd_angle;// * GMPitchRamp.Calc(&GMPitchRamp);    //加入斜坡函数
	GMPPositionPID.calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量
	//pitch speed control
	GMPSpeedPID.Ref = GMPPositionPID.Out;
	GMPSpeedPID.Fdb = 10.0*MPU6050_Real_Data.Gyro_Y;   //GYRO_Y有抖动，乘以0.1减小震荡
	GMPSpeedPID.calc(&GMPSpeedPID);
}

void GMYawControlLoop(void)
{
	GMYPositionPID.Kp = YAW_POSITION_KP_DEFAULTS + YawPositionSavedPID.kp_offset;//  gAppParamStruct.YawPositionPID.kp_offset;  //may be bug if more operation  done
	GMYPositionPID.Ki = YAW_POSITION_KI_DEFAULTS + YawPositionSavedPID.ki_offset;
	GMYPositionPID.Kd = YAW_POSITION_KD_DEFAULTS + YawPositionSavedPID.kd_offset;
	
	GMYSpeedPID.Kp = YAW_SPEED_KP_DEFAULTS + YawSpeedSavedPID.kp_offset;
	GMYSpeedPID.Ki = YAW_SPEED_KI_DEFAULTS + YawSpeedSavedPID.ki_offset;
	GMYSpeedPID.Kd = YAW_SPEED_KD_DEFAULTS + YawSpeedSavedPID.kd_offset;
	
	GMYPositionPID.calc(&GMYPositionPID);
	//yaw speed control
	GMYSpeedPID.Ref = GMYPositionPID.Out;
	GMYSpeedPID.Fdb = MPU6050_Real_Data.Gyro_Z;
	GMYSpeedPID.calc(&GMYSpeedPID);
}

void SetGimbalMotorOutput(void)
{

	//云台控制输出								
	if((GetWorkState() == STOP_STATE) || GetWorkState() == CALI_STATE)
	{
		//Set_Gimbal_Current(CanInstPtr_0, -(int16_t)GMYSpeedPID.Out, -(int16_t)GMPSpeedPID.Out);
		Set_Gimbal_Current(CanInstPtr_1, 0, 0);     //yaw + pitch
	}
	else
	{		
		Set_Gimbal_Current(CanInstPtr_1, 0, 1.0*(-(int16_t)GMPSpeedPID.Out));     //yaw + pitch
	}
}
//控制任务初始化程序
void ControtLoopTaskInit(void)
{
	//计数初始化
//	time_tick_1ms = 0;   //中断中的计数清零
	//程序参数初始化
	AppParamInit();
	//校准后参数偏差值初始化
	Sensor_Offset_Param_Init(&gAppParamStruct);
	//设置工作模式
	SetWorkState(PREPARE_STATE);
	//斜坡初始化
	//GMPitchRamp.SetScale(&GMPitchRamp, PREPARE_TIME_TICK_MS);
	//GMYawRamp.SetScale(&GMYawRamp, PREPARE_TIME_TICK_MS);
	//GMPitchRamp.ResetCounter(&GMPitchRamp);
	//GMYawRamp.ResetCounter(&GMYawRamp);
	//云台给定角度初始化
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
    //监控任务初始化
/*    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_RC));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_IMU));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_ZGYRO));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR1));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR2));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR3));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR4));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR5));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR6));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_DEADLOCK));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_NOCALI));*/
    
	//PID初始化
//	ShootMotorSpeedPID.Reset(&ShootMotorSpeedPID);
	GMPPositionPID.clear(&GMPPositionPID);
	GMPSpeedPID.clear(&GMPSpeedPID);
	GMYPositionPID.clear(&GMYPositionPID);
	GMYSpeedPID.clear(&GMYSpeedPID);
	CMRotatePID.clear(&CMRotatePID);
	CM1SpeedPID.clear(&CM1SpeedPID);
	CM2SpeedPID.clear(&CM2SpeedPID);
	CM3SpeedPID.clear(&CM3SpeedPID);
	CM4SpeedPID.clear(&CM4SpeedPID);

}

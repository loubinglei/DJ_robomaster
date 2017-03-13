#include "main.h"

/************************** Variable Definitions *****************************/
PID GMPPositionPID = PID_DEFAULTS;
//PID_Regulator_t GMPSpeedPID = GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT;
PID GMYPositionPID = PID_DEFAULTS;
//PID_Regulator_t GMYSpeedPID = GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT;

//PID_Regulator_t CMRotatePID = CHASSIS_MOTOR_ROTATE_PID_DEFAULT;
PID CM1SpeedPID = PID_DEFAULTS;
PID CM2SpeedPID = PID_DEFAULTS;
PID CM3SpeedPID = PID_DEFAULTS;
PID CM4SpeedPID = PID_DEFAULTS;

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
/*
static void SetWorkState(WorkState_e state)
{
    workState = state;
}
*/
WorkState_e GetWorkState()
{
	return workState;
}
//底盘控制任务
void CMControlLoop(void)
{  
	//底盘旋转量计算
/*	if(GetWorkState()==PREPARE_STATE) //启动阶段，底盘不旋转
	{
		ChassisSpeedRef.rotate_ref = 0;	 
	}
	else
	{
		 //底盘跟随编码器旋转PID计算
		 CMRotatePID.ref = 0;
		 CMRotatePID.fdb = GMYawEncoder.ecd_angle;
		 CMRotatePID.Calc(&CMRotatePID);   
		 ChassisSpeedRef.rotate_ref = CMRotatePID.output;
	}
	if(Is_Lost_Error_Set(LOST_ERROR_RC))      //如果遥控器丢失，强制将速度设定值reset
	{
		ChassisSpeedRef.forward_back_ref = 0;
		ChassisSpeedRef.left_right_ref = 0;
	}
*/

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

	Set_CM_Speed(CanInstPtr_1,CHASSIS_SPEED_ATTENUATION * CM1SpeedPID.Out, CHASSIS_SPEED_ATTENUATION * CM2SpeedPID.Out, CHASSIS_SPEED_ATTENUATION * CM3SpeedPID.Out, CHASSIS_SPEED_ATTENUATION * CM4SpeedPID.Out);
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

void WorkStateFSM(void)
{
/*	lastWorkState = workState;
	switch(workState)
	{
		case PREPARE_STATE:
		{
			if(GetInputMode() == STOP || Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(GetCaliCmdFlagGrp())   
			{
				workState = CALI_STATE;
			}
			else if(time_tick_1ms > PREPARE_TIME_TICK_MS)
			{
				workState = NORMAL_STATE;
			}			
		}break;
		case NORMAL_STATE:     
		{
			if(GetInputMode() == STOP || Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(GetCaliCmdFlagGrp())   
			{
				workState = CALI_STATE;
			}
			else if((!IsRemoteBeingAction() ||(Get_Lost_Error(LOST_ERROR_RC) == LOST_ERROR_RC)) && GetShootState() != SHOOTING)
			{
				workState = STANDBY_STATE;      
			}			
		}break;
		case STANDBY_STATE:     
		{
			if(GetInputMode() == STOP || Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(GetCaliCmdFlagGrp())   
			{
				workState = CALI_STATE;
			}
			else if(IsRemoteBeingAction() || (GetShootState()==SHOOTING) || GetFrictionState() == FRICTION_WHEEL_START_TURNNING)
			{
				workState = NORMAL_STATE;
			}				
		}break;
		case STOP_STATE:   
		{
			if(GetInputMode() != STOP && !Is_Serious_Error())
			{
				workState = PREPARE_STATE;   
			}
		}break;
		case CALI_STATE:      
		{
			if(GetInputMode() == STOP || Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
		}break;	    
		default:
		{
			
		}
	}	
*/
}

/*
static void WorkStateSwitchProcess(void)
{
	//如果从其他模式切换到prapare模式，要将一系列参数初始化
	if((lastWorkState != workState) && (workState == PREPARE_STATE))  
	{
		ControtLoopTaskInit();
		RemoteTaskInit();
	}
}
*/
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
/*	static uint8_t normalFlag = 0;   //正常工作模式标志
	static uint8_t standbyFlag = 1;  //IMU工作模式标志
	static uint32_t modeChangeDelayCnt = 0;
	static float angleSave = 0.0f;    //用于切换模式时保存切换前的角度值，用于角度给定值切换
	switch(GetWorkState())
	{
		case PREPARE_STATE:   //启动过程，加入斜坡
		{
			GMYPositionPID.ref = 0.0f;
			GMYPositionPID.fdb = -GMYawEncoder.ecd_angle*GMYawRamp.Calc(&GMYawRamp);
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
			GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;   //设定给定值
			GMYPositionPID.fdb = ZGyroModuleAngle; 					//设定反馈值
			angleSave = yaw_angle;   //时刻保存IMU的值用于从NORMAL向STANDBY模式切换
		}break;
		case STANDBY_STATE:   //IMU模式
		{
			modeChangeDelayCnt++;
			if(modeChangeDelayCnt < STATE_SWITCH_DELAY_TICK)    //delay的这段时间与NORMAL_STATE一样
			{
				GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;   //设定给定值
				GMYPositionPID.fdb = ZGyroModuleAngle; 					//设定反馈值
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
				GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;   //设定给定值
				GMYPositionPID.fdb = yaw_angle; 					//设定反馈值	
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
*/
}

//云台pitch轴控制程序
void GMPitchControlLoop(void)
{
/*	GMPPositionPID.kp = PITCH_POSITION_KP_DEFAULTS + PitchPositionSavedPID.kp_offset;
	GMPPositionPID.ki = PITCH_POSITION_KI_DEFAULTS + PitchPositionSavedPID.ki_offset;
	GMPPositionPID.kd = PITCH_POSITION_KD_DEFAULTS + PitchPositionSavedPID.kd_offset;
		
	GMPSpeedPID.kp = PITCH_SPEED_KP_DEFAULTS + PitchSpeedSavedPID.kp_offset;
	GMPSpeedPID.ki = PITCH_SPEED_KI_DEFAULTS + PitchSpeedSavedPID.ki_offset;
	GMPSpeedPID.kd = PITCH_SPEED_KD_DEFAULTS + PitchSpeedSavedPID.kd_offset;
	
	GMPPositionPID.ref = GimbalRef.pitch_angle_dynamic_ref;
	GMPPositionPID.fdb = -GMPitchEncoder.ecd_angle * GMPitchRamp.Calc(&GMPitchRamp);    //加入斜坡函数
	GMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量
	//pitch speed control
	GMPSpeedPID.ref = GMPPositionPID.output;
	GMPSpeedPID.fdb = MPU6050_Real_Data.Gyro_Y;
	GMPSpeedPID.Calc(&GMPSpeedPID);
*/
}

void GMYawControlLoop(void)
{
/*	GMYPositionPID.kp = YAW_POSITION_KP_DEFAULTS + YawPositionSavedPID.kp_offset;//  gAppParamStruct.YawPositionPID.kp_offset;  //may be bug if more operation  done
	GMYPositionPID.ki = YAW_POSITION_KI_DEFAULTS + YawPositionSavedPID.ki_offset;
	GMYPositionPID.kd = YAW_POSITION_KD_DEFAULTS + YawPositionSavedPID.kd_offset;
	
	GMYSpeedPID.kp = YAW_SPEED_KP_DEFAULTS + YawSpeedSavedPID.kp_offset;
	GMYSpeedPID.ki = YAW_SPEED_KI_DEFAULTS + YawSpeedSavedPID.ki_offset;
	GMYSpeedPID.kd = YAW_SPEED_KD_DEFAULTS + YawSpeedSavedPID.kd_offset;
	
	GMYPositionPID.Calc(&GMYPositionPID);
	//yaw speed control
	GMYSpeedPID.ref = GMYPositionPID.output;
	GMYSpeedPID.fdb = MPU6050_Real_Data.Gyro_Z;
	GMYSpeedPID.Calc(&GMYSpeedPID);
*/
}

void SetGimbalMotorOutput(void)
{
/*
	//云台控制输出								
	if((GetWorkState() == STOP_STATE) ||Is_Serious_Error() || GetWorkState() == CALI_STATE)   
	{
		Set_Gimbal_Current(CAN2, 0, 0);     //yaw + pitch			
	}
	else
	{		
		Set_Gimbal_Current(CAN2, -(int16_t)GMYSpeedPID.output, -(int16_t)GMPSpeedPID.output);     //yaw + pitch			
	}
*/
}
//控制任务初始化程序
void ControtLoopTaskInit(void)
{
/*	//计数初始化
	time_tick_1ms = 0;   //中断中的计数清零
	//程序参数初始化
	AppParamInit();
	//校准后参数偏差值初始化
	Sensor_Offset_Param_Init(&gAppParamStruct);
	//设置工作模式
	SetWorkState(PREPARE_STATE);
	//斜坡初始化
	GMPitchRamp.SetScale(&GMPitchRamp, PREPARE_TIME_TICK_MS);
	GMYawRamp.SetScale(&GMYawRamp, PREPARE_TIME_TICK_MS);
	GMPitchRamp.ResetCounter(&GMPitchRamp);
	GMYawRamp.ResetCounter(&GMYawRamp);
	//云台给定角度初始化
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
    //监控任务初始化
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_RC));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_IMU));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_ZGYRO));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR1));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR2));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR3));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR4));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR5));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR6));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_DEADLOCK));
    LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_NOCALI));
    
	//PID初始化
	ShootMotorSpeedPID.Reset(&ShootMotorSpeedPID);
	GMPPositionPID.Reset(&GMPPositionPID);
	GMPSpeedPID.Reset(&GMPSpeedPID);
	GMYPositionPID.Reset(&GMYPositionPID);
	GMYSpeedPID.Reset(&GMYSpeedPID);
	CMRotatePID.Reset(&CMRotatePID);
	CM1SpeedPID.Reset(&CM1SpeedPID);
	CM2SpeedPID.Reset(&CM2SpeedPID);
	CM3SpeedPID.Reset(&CM3SpeedPID);
	CM4SpeedPID.Reset(&CM4SpeedPID);
*/
}

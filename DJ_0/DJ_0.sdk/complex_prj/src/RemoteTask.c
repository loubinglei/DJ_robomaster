
#include "main.h"

RC_Ctl_t RC_CtrlData;   //remote control data
ChassisSpeed_Ref_t ChassisSpeedRef;
Gimbal_Ref_t GimbalRef;
 FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF;
//static RemoteSwitch_t switch1;   //遥控器左侧拨杆
static volatile Shoot_State_e shootState = NOSHOOTING;
static InputMode_e inputmode = REMOTE_INPUT;   //输入模式设定

//RampGen_t frictionRamp = RAMP_GEN_DAFAULT;  //摩擦轮斜坡
//RampGen_t LRSpeedRamp = RAMP_GEN_DAFAULT;   //mouse左右移动斜坡
//RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;   //mouse前后移动斜坡
 
void GetRemoteSwitchAction(RemoteSwitch_t *sw, uint8_t val)
{
	static uint32_t switch_cnt = 0;

	/* 最新状态值 */
	sw->switch_value_raw = val;
	sw->switch_value_buf[sw->buf_index] = sw->switch_value_raw;

	/* 取最新值和上一次值 */
	sw->switch_value1 = (sw->switch_value_buf[sw->buf_last_index] << 2)|
	(sw->switch_value_buf[sw->buf_index]);


	/* 最老的状态值的索引 */
	sw->buf_end_index = (sw->buf_index + 1)%REMOTE_SWITCH_VALUE_BUF_DEEP;

	/* 合并三个值 */
	sw->switch_value2 = (sw->switch_value_buf[sw->buf_end_index]<<4)|sw->switch_value1;	

	/* 长按判断 */
	if(sw->switch_value_buf[sw->buf_index] == sw->switch_value_buf[sw->buf_last_index])
	{
		switch_cnt++;	
	}
	else
	{
		switch_cnt = 0;
	}

	if(switch_cnt >= 40)
	{
		sw->switch_long_value = sw->switch_value_buf[sw->buf_index]; 	
	}

	//索引循环
	sw->buf_last_index = sw->buf_index;
	sw->buf_index++;		
	if(sw->buf_index == REMOTE_SWITCH_VALUE_BUF_DEEP)
	{
		sw->buf_index = 0;	
	}			
}
//return the state of the remote 0:no action 1:action 
/*
uint8_t IsRemoteBeingAction(void)
{
	return (fabs(ChassisSpeedRef.forward_back_ref)>=10 || fabs(ChassisSpeedRef.left_right_ref)>=10 || fabs(GimbalRef.yaw_speed_ref)>=10 || fabs(GimbalRef.pitch_speed_ref)>=10);
}
*/
//输入模式设置 
void SetInputMode(Remote *rc)  //DJ源码中为S2  此处改为S1（目的为了和遥控器说明书一致）
{
	if(rc->s1 == 1)
	{
		inputmode = REMOTE_INPUT;
	}
	else if(rc->s1 == 3)
	{
		inputmode = KEY_MOUSE_INPUT;
	}
	else if(rc->s1 == 2)
	{
		inputmode = STOP;
	}	
}

InputMode_e GetInputMode()
{
	return inputmode;
}

/*
input: RemoteSwitch_t *sw, include the switch info
*/
void RemoteShootControl(RemoteSwitch_t *sw, uint8_t val) 
{
/*
	GetRemoteSwitchAction(sw, val);
	switch(friction_wheel_state)
	{
		case FRICTION_WHEEL_OFF:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //从关闭到start turning
			{
				SetShootState(NOSHOOTING);
				frictionRamp.ResetCounter(&frictionRamp);
				friction_wheel_state = FRICTION_WHEEL_START_TURNNING;	 
//				LASER_ON();
			}				 		
		}break;
		case FRICTION_WHEEL_START_TURNNING:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //刚启动就被关闭
			{
//				LASER_OFF();
				SetShootState(NOSHOOTING);
//				SetFrictionWheelSpeed(1000);
				friction_wheel_state = FRICTION_WHEEL_OFF;
				frictionRamp.ResetCounter(&frictionRamp);
			}
			else
			{
				//摩擦轮加速
				
//				SetFrictionWheelSpeed(1000 + (FRICTION_WHEEL_MAX_DUTY-1000)*frictionRamp.Calc(&frictionRamp));
				if(frictionRamp.IsOverflow(&frictionRamp))
				{
					friction_wheel_state = FRICTION_WHEEL_ON; 	
				}
				
			}
		}break;
		case FRICTION_WHEEL_ON:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //关闭摩擦轮
			{
//				LASER_OFF();
				friction_wheel_state = FRICTION_WHEEL_OFF;				  
//				SetFrictionWheelSpeed(1000);
				frictionRamp.ResetCounter(&frictionRamp);
				SetShootState(NOSHOOTING);
			}
			else if(sw->switch_value_raw == 2)
			{
				SetShootState(SHOOTING);
			}
			else
			{
				SetShootState(NOSHOOTING);
			}					 
		} break;				
	}
*/
}
	 
void RemoteDataPrcess(uint8_t *pData)
{
    if(pData == NULL)
    {
        return;
    }
    
    RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
    RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
    RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
                         ((int16_t)pData[4] << 10)) & 0x07FF;
    RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
    
    RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x000C) >> 2;
    RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x0003);
 
    RC_CtrlData.key.v = ((int16_t)pData[14]);// | ((int16_t)pData[15] << 8);
	
	SetInputMode(&RC_CtrlData.rc);
	RemoteControlProcess(&(RC_CtrlData.rc));  //暂时不选择模式，默认遥控器模式
	
/*	switch(GetInputMode())
	{
		case REMOTE_INPUT:
		{
			//遥控器控制模式
			RemoteControlProcess(&(RC_CtrlData.rc));
		}break;
		case KEY_MOUSE_INPUT:
		{
			//键盘鼠标控制模式
		}break;
		case STOP:
		{
			//紧急停车
		}break;
	}
*/
}
//遥控器控制模式处理
void RemoteControlProcess(Remote *rc)
{
	
    ChassisSpeedRef.forward_back_ref = (rc->ch1 - (s16)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_CHASSIS_SPEED_REF_FACT;
    ChassisSpeedRef.left_right_ref   = (rc->ch0 - (s16)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_CHASSIS_SPEED_REF_FACT;

}

//
Shoot_State_e GetShootState()
{
	return shootState;
}

void SetShootState(Shoot_State_e v)
{
	shootState = v;
}

FrictionWheelState_e GetFrictionState()
{
	return friction_wheel_state;
}

void SetFrictionState(FrictionWheelState_e v)
{
	friction_wheel_state = v;
}
//遥控器输入值设置，
void GimbalAngleLimit()
{
	VAL_LIMIT(GimbalRef.pitch_angle_dynamic_ref, -PITCH_MAX+7, PITCH_MAX);
	//VAL_LIMIT(GimbalRef.yaw_angle_dynamic_ref, GMYPositionPID.Fdb - 60, GMYPositionPID.Fdb + 60);
}

//遥控器数据初始化，斜坡函数等的初始化
void RemoteTaskInit()
{
	//斜坡初始化
	//frictionRamp.SetScale(&frictionRamp, FRICTION_RAMP_TICK_COUNT);
//	LRSpeedRamp.SetScale(&LRSpeedRamp, MOUSE_LR_RAMP_TICK_COUNT);
//	FBSpeedRamp.SetScale(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
	//frictionRamp.ResetCounter(&frictionRamp);
//	LRSpeedRamp.ResetCounter(&LRSpeedRamp);
//	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	//底盘云台给定值初始化
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
	ChassisSpeedRef.rotate_ref = 0.0f;
	//摩擦轮运行状态初始化
	//SetFrictionState(FRICTION_WHEEL_OFF);
}

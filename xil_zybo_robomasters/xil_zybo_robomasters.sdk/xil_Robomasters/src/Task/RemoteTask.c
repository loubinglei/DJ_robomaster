
#include "../main.h"

RC_Ctl_t RC_CtrlData = {{0},{0},{0}};   //remote control data
ChassisSpeed_Ref_t ChassisSpeedRef;
Gimbal_Ref_t GimbalRef;

static InputMode_e inputmode = REMOTE_INPUT;   //输入模式设定

 
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

uint8_t IsRemoteBeingAction(void)
{
	return (fabs(ChassisSpeedRef.forward_back_ref)>=10 || fabs(ChassisSpeedRef.left_right_ref)>=10 || fabs(GimbalRef.yaw_speed_ref)>=10 || fabs(GimbalRef.pitch_speed_ref)>=10);
}

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
	else
	{
		inputmode = STOP;  //若未打开遥控器，初始化值为0，需停止
	}
}

InputMode_e GetInputMode()
{
	return inputmode;
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

	switch(GetInputMode())
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
}
//遥控器控制模式处理
void RemoteControlProcess(Remote *rc)
{
    if(GetWorkState()!=PREPARE_STATE)
    {
        ChassisSpeedRef.forward_back_ref = (rc->ch1 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_CHASSIS_SPEED_REF_FACT;
        ChassisSpeedRef.left_right_ref   = (rc->ch0 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_CHASSIS_SPEED_REF_FACT;
    }

    if(GetWorkState() == NORMAL_STATE)
    {
        GimbalRef.pitch_angle_dynamic_ref += (rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_PITCH_ANGLE_INC_FACT;
        GimbalRef.yaw_angle_dynamic_ref   = (rc->ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET) * STICK_TO_YAW_ANGLE_INC_FACT;
	}
	
	/* not used to control, just as a flag */
    GimbalRef.pitch_speed_ref = rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET;    //speed_ref仅作输入量判断用
    GimbalRef.yaw_speed_ref   = (rc->ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET);
	GimbalAngleLimit();

}

//遥控器输入值设置，
void GimbalAngleLimit()
{
	VAL_LIMIT(GimbalRef.pitch_angle_dynamic_ref, -36, 15);  //自己测的
	VAL_LIMIT(GimbalRef.yaw_angle_dynamic_ref, GMYPositionPID.fdb - 30, GMYPositionPID.fdb + 30);
}

//遥控器数据初始化，斜坡函数等的初始化
void RemoteTaskInit()
{
	//底盘云台给定值初始化
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
	ChassisSpeedRef.rotate_ref = 0.0f;

}

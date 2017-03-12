
#include "main.h"

RC_Ctl_t RC_CtrlData;   //remote control data
ChassisSpeed_Ref_t ChassisSpeedRef;
Gimbal_Ref_t GimbalRef;
 FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF;
//static RemoteSwitch_t switch1;   //ң������ದ��
static volatile Shoot_State_e shootState = NOSHOOTING;
static InputMode_e inputmode = REMOTE_INPUT;   //����ģʽ�趨

//RampGen_t frictionRamp = RAMP_GEN_DAFAULT;  //Ħ����б��
//RampGen_t LRSpeedRamp = RAMP_GEN_DAFAULT;   //mouse�����ƶ�б��
//RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;   //mouseǰ���ƶ�б��
 
void GetRemoteSwitchAction(RemoteSwitch_t *sw, uint8_t val)
{
	static uint32_t switch_cnt = 0;

	/* ����״ֵ̬ */
	sw->switch_value_raw = val;
	sw->switch_value_buf[sw->buf_index] = sw->switch_value_raw;

	/* ȡ����ֵ����һ��ֵ */
	sw->switch_value1 = (sw->switch_value_buf[sw->buf_last_index] << 2)|
	(sw->switch_value_buf[sw->buf_index]);


	/* ���ϵ�״ֵ̬������ */
	sw->buf_end_index = (sw->buf_index + 1)%REMOTE_SWITCH_VALUE_BUF_DEEP;

	/* �ϲ�����ֵ */
	sw->switch_value2 = (sw->switch_value_buf[sw->buf_end_index]<<4)|sw->switch_value1;	

	/* �����ж� */
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

	//����ѭ��
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
//����ģʽ���� 
void SetInputMode(Remote *rc)  //DJԴ����ΪS2  �˴���ΪS1��Ŀ��Ϊ�˺�ң����˵����һ�£�
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
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //�ӹرյ�start turning
			{
				SetShootState(NOSHOOTING);
				frictionRamp.ResetCounter(&frictionRamp);
				friction_wheel_state = FRICTION_WHEEL_START_TURNNING;	 
//				LASER_ON();
			}				 		
		}break;
		case FRICTION_WHEEL_START_TURNNING:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //�������ͱ��ر�
			{
//				LASER_OFF();
				SetShootState(NOSHOOTING);
//				SetFrictionWheelSpeed(1000);
				friction_wheel_state = FRICTION_WHEEL_OFF;
				frictionRamp.ResetCounter(&frictionRamp);
			}
			else
			{
				//Ħ���ּ���
				
//				SetFrictionWheelSpeed(1000 + (FRICTION_WHEEL_MAX_DUTY-1000)*frictionRamp.Calc(&frictionRamp));
				if(frictionRamp.IsOverflow(&frictionRamp))
				{
					friction_wheel_state = FRICTION_WHEEL_ON; 	
				}
				
			}
		}break;
		case FRICTION_WHEEL_ON:
		{
			if(sw->switch_value1 == REMOTE_SWITCH_CHANGE_3TO1)   //�ر�Ħ����
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
	RemoteControlProcess(&(RC_CtrlData.rc));  //��ʱ��ѡ��ģʽ��Ĭ��ң����ģʽ
	
/*	switch(GetInputMode())
	{
		case REMOTE_INPUT:
		{
			//ң��������ģʽ
			RemoteControlProcess(&(RC_CtrlData.rc));
		}break;
		case KEY_MOUSE_INPUT:
		{
			//����������ģʽ
		}break;
		case STOP:
		{
			//����ͣ��
		}break;
	}
*/
}
//ң��������ģʽ����
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
//ң��������ֵ���ã�
void GimbalAngleLimit()
{
	VAL_LIMIT(GimbalRef.pitch_angle_dynamic_ref, -PITCH_MAX+7, PITCH_MAX);
	//VAL_LIMIT(GimbalRef.yaw_angle_dynamic_ref, GMYPositionPID.Fdb - 60, GMYPositionPID.Fdb + 60);
}

//ң�������ݳ�ʼ����б�º����ȵĳ�ʼ��
void RemoteTaskInit()
{
	//б�³�ʼ��
	//frictionRamp.SetScale(&frictionRamp, FRICTION_RAMP_TICK_COUNT);
//	LRSpeedRamp.SetScale(&LRSpeedRamp, MOUSE_LR_RAMP_TICK_COUNT);
//	FBSpeedRamp.SetScale(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
	//frictionRamp.ResetCounter(&frictionRamp);
//	LRSpeedRamp.ResetCounter(&LRSpeedRamp);
//	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	//������̨����ֵ��ʼ��
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
	ChassisSpeedRef.rotate_ref = 0.0f;
	//Ħ��������״̬��ʼ��
	//SetFrictionState(FRICTION_WHEEL_OFF);
}
#include "../main.h"

static uint32_t can_count = 0;
u32 Rx_MESSAGE_ID = 0x200;
u32 Tx_MESSAGE_ID = 0x201;
u32 TxFrame_0[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
u32 RxFrame_0[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
u32 TxFrame_1[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
u32 RxFrame_1[XCANPS_MAX_FRAME_SIZE_IN_WORDS];


volatile Encoder CM1Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
volatile Encoder CM2Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
volatile Encoder CM3Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
volatile Encoder CM4Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
volatile Encoder GMYawEncoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
volatile Encoder GMPitchEncoder = {0,0,0,0,0,0,0,0,{0},0,0,0};

float ZGyroModuleAngle = 0.0f;

CanRxMsg rx_message_0;
CanRxMsg rx_message_1;



/*
***********************************************************************************************
*Name          :GetEncoderBias
*Input         :can message
*Return        :void
*Description   :to get the initiatial encoder of the chassis motor 201 202 203 204
*
*
***********************************************************************************************
*/

void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg)
{

            v->ecd_bias = ((msg->Data[0])<<8)|(msg->Data[1]);  //�����ʼ������ֵ��Ϊƫ��
            v->ecd_value = v->ecd_bias;
            v->last_raw_value = v->ecd_bias;
            v->temp_count++;
}

/*
***********************************************************************************************
*Name          :EncoderProcess
*Input         :can message
*Return        :void
*Description   :to get the initiatial encoder of the chassis motor 201 202 203 204
*
*
***********************************************************************************************
*/
void EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
	int i=0;
	s32 temp_sum = 0;
	v->last_raw_value = v->raw_value;
	v->raw_value = ((msg->Data[0])<<8)|(msg->Data[1]);
	v->diff = v->raw_value - v->last_raw_value;
	if(v->diff < -7500)    //���α������ķ���ֵ���̫�󣬱�ʾȦ�������˸ı�
	{
		v->round_cnt++;
		v->ecd_raw_rate = v->diff + 8192;
	}
	else if(v->diff>7500)
	{
		v->round_cnt--;
		v->ecd_raw_rate = v->diff- 8192;
	}		
	else
	{
		v->ecd_raw_rate = v->diff;
	}
	//����õ������ı��������ֵ
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	//����õ��Ƕ�ֵ����Χ���������
	v->ecd_angle = (float)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	//�����ٶ�ƽ��ֵ
	for(i = 0;i < RATE_BUF_SIZE; i++)
	{
		temp_sum += v->rate_buf[i];
	}
	v->filter_rate = (s32)(temp_sum/RATE_BUF_SIZE);
}

/*
************************************************************************************************************************
*Name        : CanReceiveMsgProcess
* Description: This function process the can message representing the encoder data received from the CAN2 bus.
* Arguments  : msg     is a pointer to the can message.
* Returns    : void
* Note(s)    : none
************************************************************************************************************************
*/
void CanReceiveMsgProcess(CanRxMsg * msg)
{      
        can_count++;
		switch(msg->StdId)
		{
				case CAN_BUS1_MOTOR1_FEEDBACK_MSG_ID:
				{
					(can_count<=50) ? GetEncoderBias(&CM1Encoder,msg):EncoderProcess(&CM1Encoder,msg);       //��ȡ���������ĳ�ʼƫ��ֵ
                    
				}break;
				case CAN_BUS1_MOTOR2_FEEDBACK_MSG_ID:
				{
					(can_count<=50) ? GetEncoderBias(&CM2Encoder,msg):EncoderProcess(&CM2Encoder,msg);
				}break;
				case CAN_BUS1_MOTOR3_FEEDBACK_MSG_ID:
				{
					(can_count<=50) ? GetEncoderBias(&CM3Encoder,msg):EncoderProcess(&CM3Encoder,msg);
				}break;
				case CAN_BUS1_MOTOR4_FEEDBACK_MSG_ID:
				{
				 	(can_count<=50) ? GetEncoderBias(&CM4Encoder,msg):EncoderProcess(&CM4Encoder,msg);
				}break;
				case CAN_BUS1_MOTOR5_FEEDBACK_MSG_ID:
				{
					 EncoderProcess(&GMYawEncoder ,msg);
						// �Ƚϱ����������ֵ��ƫ��ֵ�������������ֵ�ͳ�ʼƫ��֮���೬����ֵ����ƫ��ֵ��������ֹ������̨�������˶�
					if(GetWorkState() == PREPARE_STATE)   //׼���׶�Ҫ�����֮��Ĳ�ֵһ�����ܴ�����ֵ������϶��ǳ������ٽ��л�
					 {
							 if((GMYawEncoder.ecd_bias - GMYawEncoder.ecd_value) <-4000)
							 {
								GMYawEncoder.ecd_bias = gAppParamStruct.GimbalCaliData.GimbalYawOffset + 8192;
							 }
							 else if((GMYawEncoder.ecd_bias - GMYawEncoder.ecd_value) > 4000)
							 {
								GMYawEncoder.ecd_bias = gAppParamStruct.GimbalCaliData.GimbalYawOffset - 8192;
							 }
					 }
				}break;
				case CAN_BUS1_MOTOR6_FEEDBACK_MSG_ID:
				{
						EncoderProcess(&GMPitchEncoder ,msg);
						//�����м�ֵ�趨Ҳ��Ҫ�޸�
						 if(can_count<=100)
						 {
							 if((GMPitchEncoder.ecd_bias - GMPitchEncoder.ecd_value) <-4000)
							 {
								 GMPitchEncoder.ecd_bias = gAppParamStruct.GimbalCaliData.GimbalPitchOffset + 8192;
							 }
							 else if((GMPitchEncoder.ecd_bias - GMPitchEncoder.ecd_value) > 4000)
							 {
								 GMPitchEncoder.ecd_bias = gAppParamStruct.GimbalCaliData.GimbalPitchOffset - 8192;
							 }
						 }
				}break;

				case CAN_BUS0_ZGYRO_FEEDBACK_MSG_ID:
				{
					ZGyroModuleAngle = -0.01f*((int32_t)(msg->Data[0]<<24)|(int32_t)(msg->Data[1]<<16) | (int32_t)(msg->Data[2]<<8) | (int32_t)(msg->Data[3]));
				}break;

				default:
				{
				}
		}

}

/********************************************************************************
   �����̵���巢��ָ�ID��Ϊ0x200�������̷���IDΪ0x201-0x204
*********************************************************************************/
void Set_CM_Speed(XCanPs *InstancePtr,s16 cm1_iq, s16 cm2_iq, s16 cm3_iq, s16 cm4_iq)
{
	u8 *FramePtr;

	TxFrame_1[0] = (u32)XCanPs_CreateIdValue(0x200, 0, 0, 0, 0);
	TxFrame_1[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);
	FramePtr = (u8 *)(&TxFrame_1[2]);   // TxFrame has 4 worlds

		*FramePtr++ = (u8)(cm1_iq >> 8);
		*FramePtr++ = (u8)cm1_iq;
		*FramePtr++ = (u8)(cm2_iq >> 8);
		*FramePtr++ = (u8)cm2_iq;
		*FramePtr++ = (u8)(cm3_iq >> 8);
		*FramePtr++ = (u8)cm3_iq;
		*FramePtr++ = (u8)(cm4_iq >> 8);
		*FramePtr++ = (u8)cm4_iq;
    
    SendFrame_1(InstancePtr);

}

/********************************************************************************
   ������巢��ָ�ID��Ϊ0x1FF��ֻ����������壬���ݻش�IDΪ0x205��0x206
	 cyq:����Ϊ�������������ָ�
*********************************************************************************/
void Set_Gimbal_Current(XCanPs *InstancePtr,int16_t gimbal_yaw_iq, int16_t gimbal_pitch_iq)
{
	u8 *FramePtr;

	TxFrame_1[0] = (u32)XCanPs_CreateIdValue(0x1FF, 0, 0, 0, 0);
	TxFrame_1[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);
	FramePtr = (u8 *)(&TxFrame_1[2]);   // TxFrame has 4 worlds

		*FramePtr++ = (u8)(gimbal_yaw_iq >> 8);
		*FramePtr++ = (u8)gimbal_yaw_iq;
		*FramePtr++ = (u8)(gimbal_pitch_iq >> 8);
		*FramePtr++ = (u8)gimbal_pitch_iq;
		*FramePtr++ = 0x00;
		*FramePtr++ = 0x00;
		*FramePtr++ = 0x00;
		*FramePtr++ = 0x00;
    
    SendFrame_1(InstancePtr);

}




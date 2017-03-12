#include "main.h"

/************************** Constant Definitions *****************************/
#define TEST_BTR_SYNCJUMPWIDTH		1
#define TEST_BTR_SECOND_TIMESEGMENT	7
#define TEST_BTR_FIRST_TIMESEGMENT	15  //bps = 100M/(3+1)((15+1)+(7+1)+1)= 1Mbps
#define TEST_BRPR_BAUD_PRESCALAR	3
/************************** Variable Definitions *****************************/
u32 Rx_MESSAGE_ID = 0x200;
u32 Tx_MESSAGE_ID = 0x201;
static u32 can_count = 0;
u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];


Encoder CM1Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
Encoder CM2Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
Encoder CM3Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
Encoder CM4Encoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
//Encoder GMYawEncoder = {0,0,0,0,0,0,0,0,{0},0,0,0};
//Encoder GMPitchEncoder = {0,0,0,0,0,0,0,0,{0},0,0,0};

float ZGyroModuleAngle = 0.0f;

CanRxMsg rx_message;



int CanPs_Polled_init(u16 DeviceId, XCanPs *CanInstPtr)
{
	int Status;

	XCanPs_Config *ConfigPtr;

	/*
	 * Initialize the Can device.
	 */
	ConfigPtr = XCanPs_LookupConfig(DeviceId);
	if (CanInstPtr == NULL) {
		return XST_FAILURE;
	}
	Status = XCanPs_CfgInitialize(CanInstPtr,
					ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Enter Configuration Mode so we can setup Baud Rate Prescaler
	 * Register (BRPR) and Bit Timing Register (BTR).
	 */
	XCanPs_EnterMode(CanInstPtr, XCANPS_MODE_CONFIG);
	while(XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_CONFIG);

	/*
	 * Setup Baud Rate Prescaler Register (BRPR) and
	 * Bit Timing Register (BTR).
	 */
	XCanPs_SetBaudRatePrescaler(CanInstPtr, TEST_BRPR_BAUD_PRESCALAR);
	XCanPs_SetBitTiming(CanInstPtr, TEST_BTR_SYNCJUMPWIDTH,
				TEST_BTR_SECOND_TIMESEGMENT,
				TEST_BTR_FIRST_TIMESEGMENT);

	/*
	 * Enter Normal Mode.
	 */
	XCanPs_EnterMode(CanInstPtr, XCANPS_MODE_NORMAL);
	while(XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_NORMAL);

	return Status;
}


/*****************************************************************************/
/**
*
* Send a CAN frame.
*
* @param	InstancePtr is a pointer to the driver instance
*
* @return	XST_SUCCESS if successful, a driver-specific return code if not.
*
* @note
*
* This function waits until TX FIFO has room for at least one frame before
* sending a frame. So this function may block if the hardware is not built
* correctly.
*
******************************************************************************/
int SendFrame(XCanPs *InstancePtr)
{

	int Status;

	while (XCanPs_IsTxFifoFull(InstancePtr) == TRUE);
	Status = XCanPs_Send(InstancePtr, TxFrame);

	return Status;
}


/*****************************************************************************/
/**
*
* This function receives a frame and verifies its contents.
*
* @param	InstancePtr is a pointer to the driver instance.
*
* @return	XST_SUCCESS if successful, a driver-specific return code if not.
*
* @note
*
* This function waits until RX FIFO becomes not empty before reading a frame
* from it. So this function may block if the hardware is not built
* correctly.
*
******************************************************************************/
int RecvFrame(XCanPs *InstancePtr,CanRxMsg* RxMessage)
{
	u8 *FramePtr;
	int Status;
//	int Index;
//	u8 temp;

	while (XCanPs_IsRxEmpty(InstancePtr) == TRUE);
	Status = XCanPs_Recv(InstancePtr, RxFrame);

	if (Status == XST_SUCCESS)
	{
		FramePtr = (u8 *)(&RxFrame[2]);
		RxMessage -> StdId = (u32)0x000007FF & (RxFrame[0] >> 21);
		RxMessage -> IDE = 0;
		RxMessage -> RTR = 0;
		RxMessage -> DLC = 8;
		RxMessage -> Data[0]= *FramePtr++;
		RxMessage -> Data[1]= *FramePtr++;
		RxMessage -> Data[2]= *FramePtr++;
		RxMessage -> Data[3]= *FramePtr++;
		RxMessage -> Data[4]= *FramePtr++;
		RxMessage -> Data[5]= *FramePtr++;
		RxMessage -> Data[6]= *FramePtr++;
		RxMessage -> Data[7]= *FramePtr++;

/*		for (Index = 0; Index < FRAME_DATA_LENGTH; Index++)
		{
			temp = *FramePtr++;
			if(temp>0x0F)
			{xil_printf("%x ",temp);}
			else
			{xil_printf("0%x ",temp);}
		}
		xil_printf(" \n\r");
*/
	}
	return Status;
}




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

            v->ecd_bias = ((msg->Data[0])<<8)|(msg->Data[1]);  //保存初始编码器值作为偏差
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
	if(v->diff < -7500)    //两次编码器的反馈值差别太大，表示圈数发生了改变
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
	//计算得到连续的编码器输出值
	v->ecd_value = v->raw_value + v->round_cnt * 8192;
	//计算得到角度值，范围正负无穷大
	v->ecd_angle = (float)(v->raw_value - v->ecd_bias)*360/8192 + v->round_cnt * 360;
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	if(v->buf_count == RATE_BUF_SIZE)
	{
		v->buf_count = 0;
	}
	//计算速度平均值
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
        //GMYawEncoder.ecd_bias = yaw_ecd_bias;
        can_count++;
		switch(msg->StdId)
		{
				case CAN_BUS2_MOTOR1_FEEDBACK_MSG_ID:
				{
//					LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR1));
					(can_count<=50) ? GetEncoderBias(&CM1Encoder,msg):EncoderProcess(&CM1Encoder,msg);       //获取到编码器的初始偏差值
                    
				}break;
				case CAN_BUS2_MOTOR2_FEEDBACK_MSG_ID:
				{
//					LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR2));
					(can_count<=50) ? GetEncoderBias(&CM2Encoder,msg):EncoderProcess(&CM2Encoder,msg);
				}break;
				case CAN_BUS2_MOTOR3_FEEDBACK_MSG_ID:
				{
//					LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR3));
					(can_count<=50) ? GetEncoderBias(&CM3Encoder,msg):EncoderProcess(&CM3Encoder,msg);
				}break;
				case CAN_BUS2_MOTOR4_FEEDBACK_MSG_ID:
				{
//					LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR4));
				 	(can_count<=50) ? GetEncoderBias(&CM4Encoder,msg):EncoderProcess(&CM4Encoder,msg);
				}break;
/*				case CAN_BUS2_MOTOR5_FEEDBACK_MSG_ID:
				{
				LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR5));
					 //GMYawEncoder.ecd_bias = yaw_ecd_bias;
					 EncoderProcess(&GMYawEncoder);
						// 比较保存编码器的值和偏差值，如果编码器的值和初始偏差之间差距超过阈值，将偏差值做处理，防止出现云台反方向运动
					// if(can_count>=90 && can_count<=100)
					if(GetWorkState() == PREPARE_STATE)   //准备阶段要求二者之间的差值一定不能大于阈值，否则肯定是出现了临界切换
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
				case CAN_BUS2_MOTOR6_FEEDBACK_MSG_ID:
				{
//					LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_MOTOR6));
						//GMPitchEncoder.ecd_bias = pitch_ecd_bias;
						EncoderProcess(&GMPitchEncoder);
						//码盘中间值设定也需要修改
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
*/
/*				case CAN_BUS1_ZGYRO_FEEDBACK_MSG_ID:
				{
					LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_ZGYRO));
					ZGyroModuleAngle = -0.01f*((int32_t)(Data[0]<<24)|(int32_t)(Data[1]<<16) | (int32_t)(Data[2]<<8) | (int32_t)(Data[3]));
				}break;
*/
				default:
				{
				}
		}
		// check if deadlock, meeans the yaw angle is overflow //time should keep for a long time to avoid bug		
/*			if(!LostCounterOverflowCheck(fabs(GMYawEncoder.ecd_angle), 70.0f) || GetWorkState() == STOP_STATE)  //如果是停止模式，一直喂狗防止重新启动失败
			{
				LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_DEADLOCK));
			}
*/
}

/********************************************************************************
   给底盘电调板发送指令，ID号为0x200８底盘返回ID为0x201-0x204
*********************************************************************************/
void Set_CM_Speed(XCanPs *InstancePtr,s16 cm1_iq, s16 cm2_iq, s16 cm3_iq, s16 cm4_iq)
{
	u8 *FramePtr;

	Tx_MESSAGE_ID = 0x200;

	TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)Tx_MESSAGE_ID, 0, 0, 0, 0);
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);
	FramePtr = (u8 *)(&TxFrame[2]);   // TxFrame has 4 worlds

		*FramePtr++ = (u8)(cm1_iq >> 8);
		*FramePtr++ = (u8)cm1_iq;
		*FramePtr++ = (u8)(cm2_iq >> 8);
		*FramePtr++ = (u8)cm2_iq;
		*FramePtr++ = (u8)(cm3_iq >> 8);
		*FramePtr++ = (u8)cm3_iq;
		*FramePtr++ = (u8)(cm4_iq >> 8);
		*FramePtr++ = (u8)cm4_iq;
    
    SendFrame(InstancePtr);

}

/********************************************************************************
   给电调板发送指令，ID号为0x1FF，只用两个电调板，数据回传ID为0x205和0x206
	 cyq:更改为发送三个电调的指令。
*********************************************************************************/
void Set_Gimbal_Current(int16_t gimbal_yaw_iq, int16_t gimbal_pitch_iq)
{
	u8 *FramePtr;

	Tx_MESSAGE_ID = 0x1FF;

	TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)Tx_MESSAGE_ID, 0, 0, 0, 0);
	TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);
	FramePtr = (u8 *)(&TxFrame[2]);   // TxFrame has 4 worlds

		*FramePtr++ = (u8)(gimbal_yaw_iq >> 8);
		*FramePtr++ = (u8)gimbal_yaw_iq;
		*FramePtr++ = (u8)(gimbal_pitch_iq >> 8);
		*FramePtr++ = (u8)gimbal_pitch_iq;
		*FramePtr++ = 0x00;
		*FramePtr++ = 0x00;
		*FramePtr++ = 0x00;
		*FramePtr++ = 0x00;
    
    SendFrame(CanInstPtr_1);

}

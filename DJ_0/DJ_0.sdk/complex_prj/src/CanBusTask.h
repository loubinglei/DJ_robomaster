#ifndef __CANBUSTASK_H__
#define __CANBUSTASK_H__
/************************** Constant Definitions *****************************/
#define RATE_BUF_SIZE 6
#define XCANPS_MAX_FRAME_SIZE_IN_WORDS (XCANPS_MAX_FRAME_SIZE / sizeof(u32)) //Maximum CAN frame length in words
#define FRAME_DATA_LENGTH 		8  /* Frame Data field length */

//CAN Bus 1
#define CAN_BUS1_ZGYRO_FEEDBACK_MSG_ID            0x401
//CAN Bus 2
#define CAN_BUS2_MOTOR1_FEEDBACK_MSG_ID           0x201
#define CAN_BUS2_MOTOR2_FEEDBACK_MSG_ID           0x202
#define CAN_BUS2_MOTOR3_FEEDBACK_MSG_ID           0x203
#define CAN_BUS2_MOTOR4_FEEDBACK_MSG_ID           0x204
#define CAN_BUS2_MOTOR5_FEEDBACK_MSG_ID           0x205
#define CAN_BUS2_MOTOR6_FEEDBACK_MSG_ID           0x206

#define CAN_BUS1_ZGYRO_FEEDBACK_MSG_ID   		  0x401

typedef struct{
	s32 raw_value;
	s32 last_raw_value;
	s32 ecd_value;
	s32 diff;
	s32 temp_count;
	u8  buf_count;
	s32 ecd_bias;
	s32 ecd_raw_rate;
	s32 rate_buf[RATE_BUF_SIZE];
	s32 round_cnt;
	s32 filter_rate;
	float ecd_angle;
}Encoder;


typedef struct
{
	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */
	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */
	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be received. This parameter can be a value of
                        @ref CAN_identifier_type */
	u32 RTR;     /*!< Specifies the type of frame for the received message.
                        This parameter can be a value of
                        @ref CAN_remote_transmission_request */
	u32 DLC;     /*!< Specifies the length of the frame that will be received.
                        This parameter can be a value between 0 to 8 */
	u32 Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                       0xFF. */
	u32 FMI;     /*!< Specifies the index of the filter the message stored in
                        the mailbox passes through. This parameter can be a
                        value between 0 to 0xFF */
} CanRxMsg;



/************************** Function Prototypes ******************************/
extern int CanPs_Polled_init(u16 DeviceId, XCanPs *CanInstPtr);
extern int SendFrame(XCanPs *InstancePtr);
extern int RecvFrame(XCanPs *InstancePtr,CanRxMsg* RxMessage);

void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg);
void EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void CanReceiveMsgProcess(CanRxMsg * msg);
void Set_Gimbal_Current(s16 gimbal_pitch_iq, s16 gimbal_yaw_iq);
void Set_CM_Speed(XCanPs *InstancePtr,s16 cm1_iq, s16 cm2_iq, s16 cm3_iq, s16 cm4_iq);


/************************** Variable Definitions *****************************/
extern u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
extern u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
extern u32 Rx_MESSAGE_ID;
extern u32 Tx_MESSAGE_ID;

extern u32 StdId;
extern u8 IDE;
extern u8 RTR;
extern u8 DLC;
extern u8 Data[8];
extern u8 FMI;


Encoder CM1Encoder;
Encoder CM2Encoder;
Encoder CM3Encoder;
Encoder CM4Encoder;
Encoder GMYawEncoder;
Encoder GMPitchEncoder;
float ZGyroModuleAngle;

CanRxMsg rx_message;



#endif


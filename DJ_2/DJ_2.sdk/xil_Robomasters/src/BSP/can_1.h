#ifndef __CAN_1_H__
#define __CAN_1_H__

/************************** Constant Definitions *****************************/
#define CAN_1_INTR_VEC_ID		XPAR_XCANPS_1_INTR
#define CAN_1_DEVICE_ID			XPAR_XCANPS_1_DEVICE_ID

/************************** Function Prototypes ******************************/
extern int CanPs_Intr_init_1(INTC *IntcInstPtr, XCanPs *CanInstPtr,
		u16 CanDeviceId, u16 CanIntrId);
extern void GYRO_RST(void);
extern int SendFrame_1(XCanPs *InstancePtr);

/************************** Variable Definitions *****************************/
#define RATE_BUF_SIZE 6
typedef struct{
	int32_t raw_value;
	int32_t last_raw_value;
	int32_t ecd_value;
	int32_t diff;
	int32_t temp_count;
	uint8_t buf_count;
	int32_t ecd_bias;
	int32_t ecd_raw_rate;
	int32_t rate_buf[RATE_BUF_SIZE];
	int32_t round_cnt;
	int32_t filter_rate;
	float ecd_angle;
}Encoder;


#endif 

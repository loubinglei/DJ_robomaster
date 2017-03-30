#ifndef __CAN_0_H__
#define __CAN_0_H__

/************************** Constant Definitions *****************************/
#define CAN_0_INTR_VEC_ID		XPAR_XCANPS_0_INTR
#define CAN_0_DEVICE_ID			XPAR_XCANPS_0_DEVICE_ID

/************************** Function Prototypes ******************************/
extern int CanPs_Intr_init_0(INTC *IntcInstPtr, XCanPs *CanInstPtr,
		u16 CanDeviceId, u16 CanIntrId);
extern void GYRO_RST(void);
extern int SendFrame_0(XCanPs *InstancePtr);

/************************** Variable Definitions *****************************/


#endif 

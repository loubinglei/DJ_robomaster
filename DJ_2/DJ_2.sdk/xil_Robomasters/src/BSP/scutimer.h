#ifndef __SCUTIMER_H__
#define __SCUTIMER_H__

/************************** Constant Definitions *****************************/
#define TIMER_DEVICE_ID			XPAR_XSCUTIMER_0_DEVICE_ID
#define TIMER_INTC_DEVICE_ID	XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR			XPAR_SCUTIMER_INTR
#define TIMER_LOAD_VALUE		0x514C8

/************************** Function Prototypes ******************************/
int ScuTimerIntrExample(XScuGic *IntcInstancePtr, XScuTimer * TimerInstancePtr,
			u16 TimerDeviceId, u16 TimerIntrId);
uint32_t Get_Time_Micros(void);

/************************** Variable Definitions *****************************/
XScuTimer TimerInstance;	/* Cortex A9 Scu Private Timer Instance */
volatile int TimerExpired;

#endif

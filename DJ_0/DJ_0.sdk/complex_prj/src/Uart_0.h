#ifndef __UART_0_H__
#define __UART_0_H__

#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xscugic.h"

/************************** Constant Definitions *****************************/
#define INTC		XScuGic
#define UART_DEVICE_ID      XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_0_INTR

#define PITCH_MAX 19.0f
#define RC_FRAME_LENGTH                            18u
#define TEST_BUFFER_SIZE	18
/************************** Variable Definitions *****************************/
extern XUartPs Uart_Ps;
extern INTC InterruptController;	/* Instance of the Interrupt Controller */

u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

unsigned int ReceivedCount;
/************************** Function Prototypes ******************************/
int Uart_Intr_Initialize(INTC *IntcInstPtr, XUartPs *UartInstPtr,
		u16 DeviceId, u16 UartIntrId);

int Uart_polled_Initialize(XUartPs *UartInstPtr,u16 DeviceId);

#endif
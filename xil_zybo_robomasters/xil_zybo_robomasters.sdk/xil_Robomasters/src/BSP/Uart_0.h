#ifndef __UART_0_H__
#define __UART_0_H__

/************************** Constant Definitions *****************************/
#define UART_INT_IRQ_ID			XPAR_XUARTPS_0_INTR
#define PITCH_MAX 			19.0f
#define RC_FRAME_LENGTH     18u
#define TEST_BUFFER_SIZE	18

/************************** Variable Definitions *****************************/
extern XUartPs Uart_Ps;
extern u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
extern u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */
extern unsigned int ReceivedCount;

/************************** Function Prototypes ******************************/
int Uart_polled_Initialize(XUartPs *UartInstPtr,u16 DeviceId);

#endif

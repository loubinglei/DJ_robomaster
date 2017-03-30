
#include "../main.h"
/************************** Constant Definitions **************************/

/************************** Function Prototypes *****************************/
int Uart_polled_Initialize(XUartPs *UartInstPtr,u16 DeviceId);

/************************** Variable Definitions ***************************/
XUartPs Uart_Ps;		/* The instance of the UART Driver */

u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

unsigned int ReceivedCount;

XUartPsFormat Format_tx;



int Uart_polled_Initialize(XUartPs *UartInstPtr,u16 DeviceId)
{
	int Status;
	XUartPs_Config *Config;

	/*
	 * Initialize the UART driver so that it's ready to use
	 * Look up the configuration in the config table and then initialize it.
	 */
	Config = XUartPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XUartPs_CfgInitialize(UartInstPtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Format_tx.BaudRate=100000;
	Format_tx.DataBits=0x0;
	Format_tx.Parity=0x0;
	Format_tx.StopBits=0x0;
	Status = XUartPs_SetDataFormat(UartInstPtr, &Format_tx);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	return Status;
}


/***************************** Include Files *********************************/

#include"main.h"
//CAN BUS
#define CAN_0_DEVICE_ID		XPAR_XCANPS_0_DEVICE_ID
#define CAN_1_DEVICE_ID	    XPAR_XCANPS_1_DEVICE_ID


XCanPs Can;
//XCanPs *CanInstPtr = &Can;
XCanPs *CanInstPtr_0 = &Can;
XCanPs *CanInstPtr_1 = &Can;

/****************************************************************************/
/**
*
* This function is the main function of the Can polled example.
*
* @param	None
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note		None
*
*****************************************************************************/
int main()
{
	int Status;
	RemoteTaskInit();  //ң�������ݳ�ʼ��
	Uart_polled_Initialize(&Uart_Ps,UART_DEVICE_ID);  //UART_0��ʼ��������ң��������
	/*Uart_Intr_Initialize(&InterruptController, &Uart_Ps,\
			UART_DEVICE_ID, UART_INT_IRQ_ID);*/

	//xil_printf("CAN Polled Mode Example Test \r\n");
	Status = CanPs_Polled_init(CAN_1_DEVICE_ID, CanInstPtr_1);
	if (Status != XST_SUCCESS) {
		xil_printf("CAN Polled Mode Example Test Failed\r\n");
		return XST_FAILURE;
	}
	ScuTimerIntrExample(&IntcInstance, &TimerInstance,TIMER_DEVICE_ID, TIMER_IRPT_INTR);
	xil_printf("Initialize finished!!\n\r");
		//SendFrame(CanInstPtr_1);
		while(1)
		{
			//counter_4ms++;
			//printf("****4ms**** counter = %d\n\r",counter_4ms);
			//SendFrame(CanInstPtr_1);
			//Set_CM_Speed(CanInstPtr_1,0,0,0,0);
			/* Block receiving the buffer. */
			ReceivedCount = 0;
			while (ReceivedCount < TEST_BUFFER_SIZE) {
				ReceivedCount +=
					XUartPs_Recv(&Uart_Ps, &RecvBuffer[ReceivedCount],
						      (TEST_BUFFER_SIZE - ReceivedCount));
			}
			RemoteDataPrcess(RecvBuffer);


/*			 printf("polled %d %d %d %d %d %d %d %d %d %d\
					 %d %d %d %d %d %d %d %d %d %d\
					 %d %d %d %d %d %d\
					 \r\n",RecvBuffer[0],RecvBuffer[1],\
					 RecvBuffer[2],RecvBuffer[3],\
					 RecvBuffer[4],RecvBuffer[5],\
					 RecvBuffer[6],RecvBuffer[7],\
					 RecvBuffer[8],RecvBuffer[9],\
					 RecvBuffer[10],RecvBuffer[11],\
					 RecvBuffer[12],RecvBuffer[13],\
					 RecvBuffer[14],RecvBuffer[15],\
					 RecvBuffer[16],RecvBuffer[17],\
					 RecvBuffer[18],RecvBuffer[19],\
					 RecvBuffer[20],RecvBuffer[21],\
					 RecvBuffer[22],RecvBuffer[23],\
					 RecvBuffer[24],RecvBuffer[25]\
			);
*/
			//SendFrame(CanInstPtr_1);
		}

	return XST_SUCCESS;
}
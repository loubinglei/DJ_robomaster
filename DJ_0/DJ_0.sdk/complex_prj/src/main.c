
/***************************** Include Files *********************************/

#include"main.h"
//CAN BUS
#define CAN_0_DEVICE_ID		XPAR_XCANPS_0_DEVICE_ID
#define CAN_1_DEVICE_ID	    XPAR_XCANPS_1_DEVICE_ID


XCanPs Can;
//XCanPs *CanInstPtr = &Can;
XCanPs *CanInstPtr_0 = &Can;
XCanPs *CanInstPtr_1 = &Can;

uint32_t Upload_Speed = 1; //数据上传速度    单位 Hz
#define upload_time (1000/Upload_Speed)  //计算上传时间  单位 ms
uint32_t system_micrsecond;   //
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
	//int Status;
	ControtLoopTaskInit();
	RemoteTaskInit();  //遥控器数据初始化
	Uart_polled_Initialize(&Uart_Ps,UART_DEVICE_ID);  //UART_0初始化，接收遥控器数据
	CanPs_Polled_init(CAN_0_DEVICE_ID, CanInstPtr_0); //CAN_1初始化，底盘电机
	CanPs_Polled_init(CAN_1_DEVICE_ID, CanInstPtr_1); //CAN_1初始化，底盘电机
	IIC_Init(&Iic,IIC_0_DEVICE_ID);    //IIC_0初始化，采集MPU6050
    /*IIC_Write(MPU6050_SLAVE_ADDR,0x6B,0x0A,2);
    IIC_Write(MPU6050_SLAVE_ADDR,0x1B,0xF0,2); //gyro
    IIC_Write(MPU6050_SLAVE_ADDR,0x1C,0xF0,2);
    IIC_Write(MPU6050_SLAVE_ADDR,0x6A,0x40,2);
    IIC_Write(MPU6050_SLAVE_ADDR,0x37,0x00,2);*/
	MPU6050_Init();
	ScuTimerIntrExample(&IntcInstance, &TimerInstance,TIMER_DEVICE_ID, TIMER_IRPT_INTR);
	xil_printf("Initialize finished!!\n\r");
		//SendFrame(CanInstPtr_1);

	system_micrsecond = Get_Time_Micros();
		while(1)
		{
			IMU_getYawPitchRoll(angle);
			if((Get_Time_Micros() - system_micrsecond) > upload_time)   //1000ms 即1s执行一次
			{
				system_micrsecond = Get_Time_Micros();
				usleep(1 * 1000); //1ms
			}
			//counter_4ms++;
			//printf("****4ms**** counter = %d\n\r",123);
			//SendFrame(CanInstPtr_1);
			//Set_CM_Speed(CanInstPtr_1,0,0,0,0);
			/* Block receiving the buffer. */
/*			ReceivedCount = 0;
			while (ReceivedCount < TEST_BUFFER_SIZE) {
				ReceivedCount +=
					XUartPs_Recv(&Uart_Ps, &RecvBuffer[ReceivedCount],
						      (TEST_BUFFER_SIZE - ReceivedCount));
			}
		       RemoteDataPrcess(RecvBuffer);
*/

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

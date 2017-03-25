
/***************************** Include Files *********************************/

#include"main.h"
//CAN BUS
#define CAN_0_DEVICE_ID		XPAR_XCANPS_0_DEVICE_ID
#define CAN_1_DEVICE_ID	    XPAR_XCANPS_1_DEVICE_ID


XCanPs Can_0;
XCanPs Can_1;
XCanPs *CanInstPtr_0 = &Can_0;
XCanPs *CanInstPtr_1 = &Can_1;

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
	ControtLoopTaskInit();  //app init
	RemoteTaskInit();  //遥控器数据初始化
	usleep(800 * 1000); //800ms
	BSP_Init();
	system_micrsecond = Get_Time_Micros();
	xil_printf("Initialize finished!!\n\r");

		while(1)
		{
			IMU_getYawPitchRoll(angle);
			if((Get_Time_Micros() - system_micrsecond) > upload_time)   //1000ms 即1s执行一次
			{
				system_micrsecond = Get_Time_Micros();
				usleep(1 * 1000); //1ms
			}
		}
	return XST_SUCCESS;
}

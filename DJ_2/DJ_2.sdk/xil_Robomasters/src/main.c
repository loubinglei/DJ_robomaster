
/***************************** Include Files *********************************/
#include"main.h"

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
    init_platform();
	ControtLoopTaskInit();  //app init
	RemoteTaskInit();       //ң�������ݳ�ʼ��
	usleep(800 * 1000);     //800ms
	BSP_Init();
	system_micrsecond = Get_Time_Micros();
	xil_printf("Initialize finished!!\n\r");
	while(1)
	{
		IMU_getYawPitchRoll(angle);
		if((Get_Time_Micros() - system_micrsecond) > upload_time)   //1000ms ��1sִ��һ��
		{
			system_micrsecond = Get_Time_Micros();
			usleep(1 * 1000); //1ms
		}
	}
	cleanup_platform();
	return XST_SUCCESS;
}







#include "main.h"

void BSP_Init(void)
{
	//Uart_polled_Initialize(&Uart_Ps,UART_DEVICE_ID);  //UART_0��ʼ��������ң��������
	Uart_Intr_Initialize(&InterruptController, &Uart_Ps,\
			UART_DEVICE_ID, UART_INT_IRQ_ID);
	CanPs_Polled_init(CAN_0_DEVICE_ID, CanInstPtr_0); //CAN_0��ʼ������̨
	CanPs_Polled_init(CAN_1_DEVICE_ID, CanInstPtr_1); //CAN_1��ʼ�������̵��
	IIC_Init(&Iic,IIC_0_DEVICE_ID);    //IIC_0��ʼ�����ɼ�MPU6050
	MPU6050_Init();
	ScuTimerIntrExample(&IntcInstance, &TimerInstance,TIMER_DEVICE_ID, TIMER_IRPT_INTR);
}

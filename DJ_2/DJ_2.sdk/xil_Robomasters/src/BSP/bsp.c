#include "../main.h"

static INTC my_XScuGic;
static XCanPs Can_0;
static XCanPs Can_1;
XCanPs *CanInstPtr_0 = &Can_0;
XCanPs *CanInstPtr_1 = &Can_1;

void BSP_Init(void)
{

	GIC_init(&my_XScuGic);  							//�жϳ�ʼ��

	Uart_polled_Initialize(&Uart_Ps,UART_DEVICE_ID);	//UART_0��ʼ��������ң��������

	CanPs_Intr_init_0(&my_XScuGic,CanInstPtr_0,
			CAN_0_DEVICE_ID,CAN_0_INTR_VEC_ID);         //CAN_0��ʼ������̨

	CanPs_Intr_init_1(&my_XScuGic,CanInstPtr_1,
			CAN_1_DEVICE_ID,CAN_1_INTR_VEC_ID);         //CAN_1��ʼ�������̵��

	IIC_Init(&Iic,IIC_0_DEVICE_ID);                     //IIC_0��ʼ�����ɼ�MPU6050

	MPU6050_Init(); 									//MPU6050��ʼ��


	ScuTimerIntrExample(&my_XScuGic, &TimerInstance,
			TIMER_DEVICE_ID, TIMER_IRPT_INTR);          //1ms��ʱ���жϳ�ʼ��

}

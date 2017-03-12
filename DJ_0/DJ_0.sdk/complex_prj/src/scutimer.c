
#include "main.h"

/************************************* Function Prototypes ****************************************/
static int TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
			      XScuTimer *TimerInstancePtr, u16 TimerIntrId);

static void TimerIntrHandler(void *CallBackRef);
//static void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId);


/************************************* Function Definition ****************************************/
int ScuTimerIntrExample(XScuGic *IntcInstancePtr, XScuTimer * TimerInstancePtr,
			u16 TimerDeviceId, u16 TimerIntrId)
{
	int Status;
	//int LastTimerExpired = 0;
	XScuTimer_Config *ConfigPtr;
	/*
	 * Initialize the Scu Private Timer driver.
	 */
	ConfigPtr = XScuTimer_LookupConfig(TimerDeviceId);
	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XScuTimer_SelfTest(TimerInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*
	 * Connect the device to interrupt subsystem so that interrupts
	 * can occur.
	 */
	Status = TimerSetupIntrSystem(IntcInstancePtr,
					TimerInstancePtr, TimerIntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable Auto reload mode.
	 */
	XScuTimer_EnableAutoReload(TimerInstancePtr);

	/*
	 * Load the timer counter register.
	 */
	XScuTimer_LoadTimer(TimerInstancePtr, TIMER_LOAD_VALUE);

	/*
	 * Start the timer counter and then wait for it
	 * to timeout a number of times.
	 */
	XScuTimer_Start(TimerInstancePtr);

	return XST_SUCCESS;
}


static int TimerSetupIntrSystem(XScuGic *IntcInstancePtr,
			      XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{
	int Status;

#ifndef TESTAPP_GEN
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	Xil_ExceptionInit();
	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				IntcInstancePtr);
#endif

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId,
				(Xil_ExceptionHandler)TimerIntrHandler,
				(void *)TimerInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the device.
	 */
	XScuGic_Enable(IntcInstancePtr, TimerIntrId);

	/*
	 * Enable the timer interrupts for timer mode.
	 */
	XScuTimer_EnableInterrupt(TimerInstancePtr);

#ifndef TESTAPP_GEN
	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnable();
#endif

	return XST_SUCCESS;
}


/*
 * ��������static void TimerIntrHandler(void *CallBackRef)
 * ���ܣ��жϴ���
 * ��������scutimer.h�� ��������TIMER_LOAD_VALUE���ڴ�����Ϊ0x514C8 ��ΪCPUƵ��Ϊ666Mhz ������Ƶ��Ϊ��һ��333MHZ  ͨ����Ƶ��ʽ ����1MS�жϣ������ж�1ms����һ��
 * ˵����IIC�ɼ�MPU6050���ݣ����������βɼ����ʱ��϶̣��ɼ������д��������ڴ����βɼ��ŵ�6��1ms�н��У��Ա�֤���ݲɼ���ȷ
 *
 *
 */

static void TimerIntrHandler(void *CallBackRef)
{
	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
    static int count_1ms=0;

	if (XScuTimer_IsExpired(TimerInstancePtr))
	{
		    XScuTimer_ClearInterruptStatus(TimerInstancePtr);

		    count_1ms++;  //counter + 1
		    //xil_printf("***********1ms**************\n\r");
		    if(count_1ms % 1000 ==0) //1S��һ��    ���ڴ��ڴ�ӡ
		    {
		    	//��ӡң��������
				printf("ch0 = %x ch1 = %x ch2 = %x ch3 = %x\n",\
						RC_CtrlData.rc.ch0,RC_CtrlData.rc.ch1,RC_CtrlData.rc.ch2,RC_CtrlData.rc.ch3);
				printf("s1 = %x s2 = %x\n",RC_CtrlData.rc.s1,RC_CtrlData.rc.s2);
				printf("key = %x\n",RC_CtrlData.key.v);
				xil_printf("*************************************************************\n\r");
		    }
		    if(count_1ms % 4 ==0) //4mS��һ��        4ms == motor control frequency
		    {
		    	//���̿�������
			   CMControlLoop();
		    }
		    if(count_1ms % 4 ==2)
		    {
				RecvFrame(CanInstPtr_1,&rx_message);   //����can
				CanReceiveMsgProcess(&rx_message);    //����can���յ�����
		    }
	}
}
/*
static void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId)
{

	 //Disconnect and disable the interrupt for the Timer.

	XScuGic_Disconnect(IntcInstancePtr, TimerIntrId);
}
*/
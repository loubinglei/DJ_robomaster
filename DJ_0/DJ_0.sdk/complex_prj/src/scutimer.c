
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
 * 函数名：static void TimerIntrHandler(void *CallBackRef)
 * 功能：中断处理
 * 参数：在scutimer.h中 可以设置TIMER_LOAD_VALUE，在此设置为0x514C8 因为CPU频率为666Mhz 计数器频率为其一半333MHZ  通过分频公式 设置1MS中断，即此中断1ms进入一次
 * 说明：IIC采集MPU6050数据，若连续两次采集相隔时间较短，采集数据有错误，所以在此六次采集放到6个1ms中进行，以保证数据采集正确
 *
 *
 */
static u32 count_1ms_ext=0;
int i=0;
u8 RecvBuffer_clear[TEST_BUFFER_SIZE];
static void TimerIntrHandler(void *CallBackRef)
{
	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
    static u32 count_1ms=0;

	if (XScuTimer_IsExpired(TimerInstancePtr))
	{
		    XScuTimer_ClearInterruptStatus(TimerInstancePtr);
		    count_1ms_ext = count_1ms;
		    count_1ms++;  //counter + 1

			WorkStateFSM(count_1ms);
			WorkStateSwitchProcess();

		    GetPitchYawGxGyGz();  //读取姿态数据，数据已处理成连续方式

			//启动后根据磁力计的数据初始化四元数
			if(count_1ms <100)
			{
				Init_Quaternion();                                       //OK
			}
			//平台稳定后，复位陀螺仪模块
			if(count_1ms == PREPARE_TIME_TICK_MS/2)
			{
				GYRO_RST();                                              //OK
			}



			//step 1: 云台控制
			GimbalYawControlModeSwitch();   //模式切换处理，得到位置环的设定值和给定值
			GMPitchControlLoop();
			GMYawControlLoop();
			SetGimbalMotorOutput();

		    if(count_1ms % 1000 ==0) //1S进一次    用于串口打印
		    {

		    	//打印遥控器数据
	//			printf("ch0 = %d ch1 = %d ch2 = %d ch3 = %d\n",\
						RC_CtrlData.rc.ch0,RC_CtrlData.rc.ch1,RC_CtrlData.rc.ch2,RC_CtrlData.rc.ch3);
	//			printf("s1 = %d s2 = %d\n",RC_CtrlData.rc.s1,RC_CtrlData.rc.s2);
	//			printf("key = %d\n",RC_CtrlData.key.v);
				//xil_printf("*************************************************************\n\r");

	//		   	printf("CM1SpeedPID = %f CM2SpeedPID = %f CM3SpeedPID = %f CM4SpeedPID = %f\n\r",CM1SpeedPID.Out,\
			   			CM2SpeedPID.Out,\
						CM3SpeedPID.Out,\
						CM4SpeedPID.Out);
	//		   	printf("CM1SpeedPID.Fdb = %f CM2SpeedPID.Fdb = %f CM3SpeedPID.Fdb = %f CM4SpeedPID.Fdb = %f\n\r",CM1SpeedPID.Fdb,\
			   			CM2SpeedPID.Fdb,\
						CM3SpeedPID.Fdb,\
						CM4SpeedPID.Fdb);
/*		   	printf("angle[0] = %f angle[1] = %f angle[2] = %f\n\r",angle[0],\
		   			angle[1],\
					angle[2]);
		   	printf("Pitch_Ref = %f Pitch_Fdb = %f Pitch_Out = %f\n\r",GMPSpeedPID.Ref,\
		   			GMPSpeedPID.Fdb,\
					GMPSpeedPID.Out);
		   	printf("pitch_angle_dynamic_ref = %f\n\r",GimbalRef.pitch_angle_dynamic_ref);

			printf("GMPPositionPID.Fdb = %f\n\r",GMPPositionPID.Fdb);
*/
//printf("MPU6050_Real_Data.Gyro_Y = %f\n\r",MPU6050_Real_Data.Gyro_Y);
//printf("-GMPitchEncoder.ecd_angle = %f\n\r",-GMPitchEncoder.ecd_angle);
//    xil_printf("*************************************************************\n\r");
		    }
			//chassis motor control
		    if(count_1ms % 4 ==0) //4mS进一次        4ms == motor control frequency
		    {
		    	//底盘控制任务
			   CMControlLoop();
		    }
		    if(count_1ms % 4 ==1)
		    {
				RecvFrame(CanInstPtr_1,&rx_message_1);   //接收can_1
				CanReceiveMsgProcess(&rx_message_1);    //处理can_1接收的数据
	/*			printf("CMxSpeedPID.Ref = %f %f %f %f\n",CM1SpeedPID.Ref,\
						CM2SpeedPID.Ref,\
						CM3SpeedPID.Ref,\
						CM4SpeedPID.Ref);
*/
/*				printf("CMxSpeedPID.Fdb = %f %f %f %f\n",CM1SpeedPID.Fdb,\
						CM2SpeedPID.Fdb,\
						CM3SpeedPID.Fdb,\
						CM4SpeedPID.Fdb);
*/
		    }
		    if(count_1ms % 4 ==2)
		    {
				RecvFrame(CanInstPtr_0,&rx_message_0);   //接收can_0
				CanReceiveMsgProcess(&rx_message_0);    //处理can_0接收的数据
		    }
		    if(count_1ms % 4 ==3)
		    {
				//	接收遥控器数据
				if(ReceivedCount>=TEST_BUFFER_SIZE)
				{
					ReceivedCount = 0;
				}
				ReceivedCount +=
						XUartPs_Recv(&Uart_Ps, &RecvBuffer[ReceivedCount],
									(TEST_BUFFER_SIZE - ReceivedCount));
			    RemoteDataPrcess(RecvBuffer);

		    }
	}
}

uint32_t Get_Time_Micros(void)
{
	return count_1ms_ext;
}
/*
static void TimerDisableIntrSystem(XScuGic *IntcInstancePtr, u16 TimerIntrId)
{

	 //Disconnect and disable the interrupt for the Timer.

	XScuGic_Disconnect(IntcInstancePtr, TimerIntrId);
}
*/

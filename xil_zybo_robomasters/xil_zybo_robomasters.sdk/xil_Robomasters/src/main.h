#ifndef __MAIN_H__
#define __MAIN_H__

/***************************** Include Files *********************************/
#include "platform.h"
#include "xcanps.h"
#include "xparameters.h"
#include "xil_printf.h"
#include "stdio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xuartps.h"
#include "xplatform_info.h"
#include "xiicps.h"
#include "math.h"
#include "sleep.h"
#include "xscutimer.h"
#include "xscugic.h"

//RMLib.lib
#include "./RMLib/common.h"
#include "./RMLib/pid_regulator.h"
#include "./RMLib/ramp.h"

//BSP
#include "./BSP/GIC_init.h"
#include "./BSP/can_0.h"
#include "./BSP/can_1.h"
#include "./BSP/bsp.h"
#include "./BSP/scutimer.h"
#include "./BSP/Uart_0.h"
#include "./BSP/IIC_API.h"

//Task
#include "./Task/CanBusTask.h"
#include "./Task/RemoteTask.h"
#include "./Task/ControlTask.h"
#include "./Task/IOTask.h"

//MPU6050
#include "./MPU6050/imu.h"
#include "./MPU6050/mpu6050_driver.h"

/************************** Constant Definitions *****************************/
#define upload_time 1000 //计算上传时间  单位 ms

/************************** Variable Definitions *****************************/
uint32_t system_micrsecond;

#endif

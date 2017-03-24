#ifndef __MAIN_H__
#define __MAIN_H__
/***************************** Include Files *********************************/
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


#include "scutimer.h"
#include "Uart_0.h"
#include "RemoteTask.h"
//#include "ramp.h"
#include "ControlTask.h"
//#include "pid_regulator.h"

#include "CanBusTask.h"
#include "pid.h"
#include "common.h"

#include "imu.h"
#include "mpu6050_driver.h"
#include "IIC_API.h"
#include "IOTask.h"

/************************** Constant Definitions *****************************/


/************************** Variable Definitions *****************************/
extern XCanPs *CanInstPtr_0;
extern XCanPs *CanInstPtr_1;

#endif

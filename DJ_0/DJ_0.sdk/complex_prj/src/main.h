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

#include "scutimer.h"
#include "Uart_0.h"
#include "RemoteTask.h"
//#include "ramp.h"
#include "ControlTask.h"
//#include "pid_regulator.h"

#include "CanBusTask.h"
#include "pid.h"
#include "common.h"

/************************** Constant Definitions *****************************/


/************************** Variable Definitions *****************************/
extern XCanPs *CanInstPtr_0;
extern XCanPs *CanInstPtr_1;

#endif
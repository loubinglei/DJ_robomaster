#ifndef __CONTROL_TASK_H__
#define __CONTROL_TASK_H__

#include "pid.h"


/************************** Constant Definitions *****************************/
//initiate status: 
typedef enum
{
    PREPARE_STATE,     		//上电后初始化状态 4s钟左右
    STANDBY_STATE,			//云台停止不转状态
    NORMAL_STATE,			//无输入状态
    STOP_STATE,        	//停止运动状态
    CALI_STATE,    			//校准状态
}WorkState_e;

#define PREPARE_TIME_TICK_MS 4000      //prapare time in ms

#define STATE_SWITCH_DELAY_TICK 100000   //mode change delay count in ms

#define YAW_POSITION_KP_DEFAULTS  0                                //25
#define YAW_POSITION_KI_DEFAULTS  0
#define YAW_POSITION_KD_DEFAULTS  0

#define YAW_SPEED_KP_DEFAULTS  0                                        //35
#define YAW_SPEED_KI_DEFAULTS  0
#define YAW_SPEED_KD_DEFAULTS  0

// avoid bang --->  position:20.0  speed:19.0
//big bang   22.5 20.0
#define PITCH_POSITION_KP_DEFAULTS  20.0                                   //20
#define PITCH_POSITION_KI_DEFAULTS  0
#define PITCH_POSITION_KD_DEFAULTS  0.1

#define PITCH_SPEED_KP_DEFAULTS  12.0                                 //19.0
#define PITCH_SPEED_KI_DEFAULTS  0
#define PITCH_SPEED_KD_DEFAULTS  0.1

#define PID_SHOOT_MOTOR_SPEED_INIT (7)
#define PID_SHOOT_MOTOR_SPEED      (10)
#define PID_SHOOT_MOTOR_CHECK_SPEED (3)

#define CHASSIS_SPEED_ATTENUATION   (1.0f)


#define CHASSIS_MOTOR_ROTATE_PID_DEFAULT\
						 { 0.0f, \
                           0.0f, \
                           0.0f, \
						   12.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   1000.0f, \
						   1500.0f, \
                           0.0f, \
                           5000.0f, \
                           -5000.0f, \
                           0.0f, \
                           0.0f, \
						   0.0f, \
                           0.0f, \
						   0.0f, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}\

//gimbal position pid control
//20  19
#define GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT\
						 { 0.0f, \
                           0.0f, \
                           0.0f, \
						   PITCH_POSITION_KP_DEFAULTS, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   1000.0f, \
						   1500.0f, \
                           0.0f, \
                           4900.0f, \
                           -4900.0f, \
                           0.0f, \
                           0.0f, \
						   PITCH_POSITION_KI_DEFAULTS, \
                           0.0f, \
						   PITCH_POSITION_KD_DEFAULTS, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}\

//gimbal speed pid control
#define GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT\
						 { 0.0f, \
                           0.0f, \
                           0.0f, \
						   PITCH_SPEED_KP_DEFAULTS, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   1000.0f, \
						   1500.0f, \
                           0.0f, \
                           4900.0f, \
                           -4900.0f, \
                           0.0f, \
                           0.0f, \
						   PITCH_SPEED_KI_DEFAULTS, \
                           0.0f, \
						   PITCH_SPEED_KD_DEFAULTS, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}\
//gimbal yaw position pid control
#define GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT\
						 { 0.0f, \
                           0.0f, \
                           0.0f, \
						   YAW_POSITION_KP_DEFAULTS, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   1000.0f, \
						   1500.0f, \
                           0.0f, \
                           5000.0f, \
                           -5000.0f, \
                           0.0f, \
                           0.0f, \
						   YAW_POSITION_KI_DEFAULTS, \
                           0.0f, \
						   YAW_POSITION_KD_DEFAULTS, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}\
//gimbal yaw speed pid control
#define GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT\
						 { 0.0f, \
                           0.0f, \
                           0.0f, \
						   YAW_SPEED_KP_DEFAULTS, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   1000.0f, \
						   1500.0f, \
                           0.0f, \
						   4900.0f, \
                           -4900.0f, \
                           0.0f, \
                           0.0f, \
						   YAW_SPEED_KI_DEFAULTS, \
                           0.0f, \
						   YAW_SPEED_KD_DEFAULTS, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}\
//D参数原来为0.4
#define CHASSIS_MOTOR_SPEED_PID_DEFAULT\
						 { 0.0f, \
                           0.0f, \
                           0.0f, \
						   1.2f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
						   4900.0f, \
						   3500.0f, \
						   1500.0f, \
                           0.0f, \
                           4950.0f, \
                           -4950.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
                           0.0f, \
              			  &pid_calc,\
											&pid_clear}\


#define SHOOT_MOTOR_POSITION_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	220.f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	4900,\
	3500,\
	1500,\
	0,\
	4950,\
	0,\
	0,\
	0,\
	&PID_Calc,\
	&PID_Reset,\
}\

#define SHOOT_MOTOR_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	50.0f,\
	0.5f,\
	0.0f,\
	0,\
	0,\
	0,\
	1000,\
	200,\
	100,\
	0,\
	4950,\
	0,\
	0,\
	0,\
	&PID_Calc,\
	&PID_Reset,\
}\

#define PID_CALI_DEFAULT  \
{\
	0,\
	0,\
	0,\
	0,\
}\

/************************** Function Prototypes ******************************/
WorkState_e GetWorkState(void);
void ControtLoopTaskInit(void);
void WorkStateFSM(int time_tick_1ms);
void WorkStateSwitchProcess(void);
void GMPitchControlLoop(void);
void GMYawControlLoop(void);
void GMPitchControlLoop(void);
void GimbalYawControlModeSwitch(void);   //模式切换处理，得到位置环的设定值和给定值
void SetGimbalMotorOutput(void);
void CMControlLoop(void);

/************************** Variable Definitions *****************************/
extern PID CMRotatePID;      		//底盘跟随云台PID
extern PID GMPPositionPID;      	//pitch轴位置环PID
extern PID GMPSpeedPID;      		//pitch轴速度换PID
extern PID GMYPositionPID;			//yaw轴位置环PID
extern PID GMYSpeedPID;      		//yaw轴速度环PID

//extern PID ShootMotorPositionPID;    //射击电机位置环PID
//extern PID ShootMotorSpeedPID;       //射击电机速度环PID

extern PID CM1SpeedPID;				 //底盘电机速度环PID
extern PID CM2SpeedPID;
extern PID CM3SpeedPID;
extern PID CM4SpeedPID;


#endif


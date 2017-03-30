#include "../main.h"

AppParam_t gAppParamStruct;	//配置信息,这里保存着最新的校准值，并且与Flash中的内容同步
PIDParamStruct_t PIDCaliData;  //保存pitch轴position校准值
//这几个变量用于在实际程序中应用
GimbalCaliStruct_t GimbalSavedCaliData;    	    //gimbal pitch yaw encoder offset
GyroCaliStruct_t GyroSavedCaliData;     	    //gyro offset data
AccCaliStruct_t AccSavedCaliData;    	    	//ACC offset data
MagCaliStruct_t MagSavedCaliData;			    //Mag offset data
PIDParamStruct_t PitchPositionSavedPID;        	//PID offset data
PIDParamStruct_t PitchSpeedSavedPID;        	//PID offset data
PIDParamStruct_t YawPositionSavedPID;        	//PID offset data
PIDParamStruct_t YawSpeedSavedPID;        	    //PID offset data

uint8_t app_param_calied_flag = 0;



void AppParamInit(void)
{
     gAppParamStruct.ParamSavedFlag=0x5A;
     gAppParamStruct.FirmwareVersion=0x00000000;

     gAppParamStruct.GimbalCaliData.GimbalCaliFlag = PARAM_SAVED_FLAG;
     gAppParamStruct.GimbalCaliData.GimbalPitchOffset = 0x096C;
     gAppParamStruct.GimbalCaliData.GimbalYawOffset = 0x0DC4;
    
     gAppParamStruct.GyroCaliData.GyroCaliFlag = PARAM_SAVED_FLAG;
     gAppParamStruct.GyroCaliData.GyroXOffset = 0xFFAC;
     gAppParamStruct.GyroCaliData.GyroYOffset = 0x0054;
     gAppParamStruct.GyroCaliData.GyroZOffset = 0x001C;
    
     gAppParamStruct.AccCaliData.AccCaliFlag = PARAM_CALI_NONE;
     gAppParamStruct.AccCaliData.AccXOffset = 0x0000;
     gAppParamStruct.AccCaliData.AccYOffset = 0x0000;
     gAppParamStruct.AccCaliData.AccZOffset = 0x0000;
     gAppParamStruct.AccCaliData.AccXScale = 1.0;
     gAppParamStruct.AccCaliData.AccYScale = 1.0;
     gAppParamStruct.AccCaliData.AccZScale = 1.0;
    
     gAppParamStruct.MagCaliData.MagCaliFlag = PARAM_CALI_NONE;
     gAppParamStruct.MagCaliData.MagXOffset = 0x0000;
     gAppParamStruct.MagCaliData.MagYOffset = 0x0000;
     gAppParamStruct.MagCaliData.MagZOffset = 0x0000;
     gAppParamStruct.MagCaliData.MagXScale = 1.0;
     gAppParamStruct.MagCaliData.MagYScale = 1.0;
     gAppParamStruct.MagCaliData.MagZScale = 1.0;

     app_param_calied_flag = 1;
}

void Sensor_Offset_Param_Init(AppParam_t *appParam)
{
	memcpy(&MagSavedCaliData, &(appParam->MagCaliData), sizeof((appParam->MagCaliData)));
	memcpy(&GyroSavedCaliData, &(appParam->GyroCaliData), sizeof((appParam->GyroCaliData)));
	memcpy(&GimbalSavedCaliData, &(appParam->GimbalCaliData), sizeof((appParam->GimbalCaliData)));
	
	memcpy(&PitchPositionSavedPID, &(appParam->PitchPositionPID), sizeof((appParam->PitchPositionPID)));
	memcpy(&PitchSpeedSavedPID, &(appParam->PitchSpeedPID), sizeof((appParam->PitchSpeedPID)));
	memcpy(&YawPositionSavedPID, &(appParam->YawPositionPID), sizeof((appParam->YawPositionPID)));
	memcpy(&YawSpeedSavedPID, &(appParam->YawSpeedPID), sizeof((appParam->YawSpeedPID)));

	GMPitchEncoder.ecd_bias = gAppParamStruct.GimbalCaliData.GimbalPitchOffset;
	GMYawEncoder.ecd_bias = gAppParamStruct.GimbalCaliData.GimbalYawOffset;	
}



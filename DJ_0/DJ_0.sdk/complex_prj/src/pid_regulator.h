#ifndef __PID_REGULATOR_H__
#define __PID_REGULATOR_H__



#define RATE_BUF_SIZE 6
typedef struct{
	int32_t raw_value;
	int32_t last_raw_value;
	int32_t ecd_value;
	int32_t diff;
	int32_t temp_count;
	uint8_t buf_count;
	int32_t ecd_bias;
	int32_t ecd_raw_rate;
	int32_t rate_buf[RATE_BUF_SIZE];
	int32_t round_cnt;
	int32_t filter_rate;
	float ecd_angle;
}Encoder;





typedef struct PID_Regulator_t
{
	float ref;
	float fdb;
	float err[2];
	float kp;
	float ki;
	float kd;
	float componentKp;
	float componentKi;
	float componentKd;
	float componentKpMax;
	float componentKiMax;
	float componentKdMax;
	float output;
	float outputMax;
	float kp_offset;
	float ki_offset;
	float kd_offset;
	void (*Calc)(struct PID_Regulator_t *pid);//����ָ��
	void (*Reset)(struct PID_Regulator_t *pid);
}PID_Regulator_t;
void PID_Reset(PID_Regulator_t *pid);
void PID_Calc(PID_Regulator_t *pid);
#endif


#include "pid.h"

void PID_SetPIDPara(PID_TypeDef *PID_InitStruct, float KP, float KI, float KD)
{
	PID_InitStruct->kp = KP;
	PID_InitStruct->ki = KI;
	PID_InitStruct->kd = KD;
}


void PID_SetThresPara(PID_TypeDef *PID_InitStruct, float upThres, float downThres)
{
	PID_InitStruct->upThres = upThres;
	PID_InitStruct->downThres = downThres;
}


void PID_SetIntegLimiting(PID_TypeDef *PID_InitStruct, float maxInteg, float minInteg)
{
	PID_InitStruct->maxInteg = maxInteg;
	PID_InitStruct->minInteg = minInteg;
}


void PID_SetDiffLimiting(PID_TypeDef *PID_InitStruct, float maxDiff, float minDiff)
{
	PID_InitStruct->maxInteg = maxDiff;
	PID_InitStruct->minInteg = minDiff;
}


void PID_Init(PID_TypeDef *PID_InitStruct)
{
	PID_InitStruct->error = 0.0;
	PID_InitStruct->error_last = 0.0;
	PID_InitStruct->integral = 0.0;
	PID_InitStruct->differential = 0.0;
	PID_InitStruct->output = 0.0;

	//将PID参数初始化为宏定义确定的默认通用参数
	PID_SetPIDPara(PID_InitStruct, PID_DEFAULT_P, PID_DEFAULT_I, PID_DEFAULT_D);
	PID_SetThresPara(PID_InitStruct, PID_DEFAULT_UPTHRES, PID_DEFAULT_DOWNTHRES);
	PID_SetIntegLimiting(PID_InitStruct, PID_DEFAULT_MAXINTEG, PID_DEFAULT_MININTEG);
	PID_SetDiffLimiting(PID_InitStruct, PID_DEFAULT_MAXDIFF, PID_DEFAULT_MINDIFF);
}


/**
 * @brief 静态函数，用来计算一次P值，I值，D值
 * @param PID_InitStruct: 传入的PID类型定义的结构体指针
 * @return 计算的结果，经过限幅处理，范围为0~1
 */
static float PID_Calculate(PID_TypeDef *PID_InitStruct)
{
	float pid_output = 0;

	///当输出限幅的时候，积分累加部分也应同时进行限幅，以防输出不变而积分项继续累加，也即所谓的积分饱和过深。
	PID_InitStruct->integral += PID_InitStruct->error;
	//积分量限幅
	if(PID_InitStruct->integral >PID_InitStruct->maxInteg)
	{
		PID_InitStruct->integral = PID_InitStruct->maxInteg ;
	}
	else if(PID_InitStruct->integral < PID_InitStruct->minInteg)
	{
		PID_InitStruct->integral = PID_InitStruct->minInteg ;
	}
	else
	{

	}

	PID_InitStruct->differential = PID_InitStruct->error - PID_InitStruct->error_last;
	//微分量限幅限制
	if(PID_InitStruct->differential > PID_InitStruct->maxDiff)
	{
		PID_InitStruct->differential = PID_InitStruct->maxDiff;
	}
	else if(PID_InitStruct->differential < PID_InitStruct->minDiff)
	{
		PID_InitStruct->differential = PID_InitStruct->minDiff;
	}
	else
	{

	}

	pid_output = PID_InitStruct->kp * PID_InitStruct->error + PID_InitStruct->ki * PID_InitStruct->integral
			+ PID_InitStruct->kd * PID_InitStruct->differential;

	if (pid_output > 1)
	{
		pid_output = 1;
	}
	else if (pid_output < 0)
	{
		pid_output = 0;
	}
	else
	{

	}

	return pid_output;
}


float PID_Update(PID_TypeDef *PID_InitStruct,  float SV, float PV)
{
	PID_InitStruct->sv = SV;
	PID_InitStruct->pv = PV;

	PID_InitStruct->error_last = PID_InitStruct->error;
	PID_InitStruct->error = PID_InitStruct->sv - PID_InitStruct->pv;

	///PV < SV - PIDDownThres. PID use full power.
	if(PID_InitStruct->error > PID_InitStruct->downThres)
	{
		PID_InitStruct->output = 1;
	}
	///PV < SV + PIDUpThres. PID use calculated data.
	else if(PID_InitStruct->error > 0 - PID_InitStruct->upThres)
	{
		PID_InitStruct->output = PID_Calculate(PID_InitStruct);
	}
	///PV >= SV + PIDUpThres. PID use zero power.
	else
	{
		PID_InitStruct->output = 0;
	}

	return PID_InitStruct->output;
}


void PID_Reset(PID_TypeDef *PID_InitStruct)
{
	PID_InitStruct->sv = 0;
	PID_InitStruct->pv = 0;
	PID_InitStruct->kp = 0;
	PID_InitStruct->ki = 0;
	PID_InitStruct->kd = 0;
	PID_InitStruct->error = 0;
	PID_InitStruct->error_last = 0;
	PID_InitStruct->integral = 0;
	PID_InitStruct->differential = 0;
	PID_InitStruct->output = 0;
}


float PID_GetOutput(PID_TypeDef *PID_InitStruct)
{
	return PID_InitStruct->output;
}


float PID_GetError(PID_TypeDef *PID_InitStruct)
{
	return PID_InitStruct->error;
}


float PID_GetIntegral(PID_TypeDef *PID_InitStruct)
{
	return PID_InitStruct->integral;
}


float PID_GetDifferential(PID_TypeDef *PID_InitStruct)
{
	return PID_InitStruct->differential;
}

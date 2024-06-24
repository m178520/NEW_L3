#ifndef __PID_H__
#define __PID_H__


typedef struct
{
	float sv;
	float pv;
	float kp;
	float ki;
	float kd;
	float error;
	float error_last;
	float integral;
	float differential;
	float output;

	float upThres;			//PID启动上阈值
	float downThres;		//PID启动下阈值

	float maxInteg;			//最大积分限幅值（正值）
	float minInteg;			//最小积分限幅值（负值）
	float maxDiff;			//最大微分限幅值
	float minDiff;			//最小微分限幅值
}PID_TypeDef;

#define PID_DEFAULT_P						1.0
#define PID_DEFAULT_I						0.0
#define PID_DEFAULT_D						0.0

#define PID_DEFAULT_UPTHRES			3.0
#define PID_DEFAULT_DOWNTHRES		3.0

#define PID_DEFAULT_MAXINTEG			10.0
#define PID_DEFAULT_MININTEG			-10.0
#define PID_DEFAULT_MAXDIFF			10.0
#define PID_DEFAULT_MINDIFF				-10.0


void PID_SetPIDPara(PID_TypeDef *PID_InitStruct, float KP, float KI, float KD);
void PID_SetThresPara(PID_TypeDef *PID_InitStruct, float upThres, float downThres);
void PID_SetIntegLimiting(PID_TypeDef *PID_InitStruct, float maxInteg, float minInteg);
void PID_SetDiffLimiting(PID_TypeDef *PID_InitStruct, float maxDiff, float minDiff);

void PID_Init(PID_TypeDef *PID_InitStruct);
float PID_Update(PID_TypeDef *PID_InitStruct,  float SV, float PV);
void PID_Reset(PID_TypeDef *PID_InitStruct);
float PID_GetOutput(PID_TypeDef *PID_InitStruct);
float PID_GetError(PID_TypeDef *PID_InitStruct);
float PID_GetIntegral(PID_TypeDef *PID_InitStruct);
float PID_GetDifferential(PID_TypeDef *PID_InitStruct);

#endif // __PID__H


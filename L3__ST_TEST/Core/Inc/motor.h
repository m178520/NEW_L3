#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stdint.h"

// ��Ҫ���ٶ�����ֵ
#define CANSpeedStop 0
#define CANSpeedLow  -800
#define CANSpeedHigh 800 

#define NALNewMin -600 // �������ƣ������ٶȱ�֤��ȫ ���ʵ�����
#define NALNewMax  600 // �����ٶȱ�֤��ȫ 

#define NALOldMin -190
#define NALOldMax  190

// ң�������ģʽ��ͨ������ֵ
#define SbusLow				-950
#define SbusHigh			950

// ������ID
#define DRIVEID 			0x601
#define ELECMAC_LIFT  0x02
#define ELECMAC_RIGHT 0x01

typedef struct
{
	int8_t L_Msg[8];
	int8_t R_Msg[8];
}CAN_Msg_t;


void vcu_Trans_LGD(double Angle,double Speed);
CAN_Msg_t Direct_Drive_motor(int16_t RSpeed, int16_t LSpeed);
	
#endif


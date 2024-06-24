#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stdint.h"

// 想要的速度区间值
#define CANSpeedStop 0
#define CANSpeedLow  -800
#define CANSpeedHigh 800 

#define NALNewMin -600 // 导航控制，降低速度保证安全 可适当增加
#define NALNewMax  600 // 降低速度保证安全 

#define NALOldMin -190
#define NALOldMax  190

// 遥控器混控模式下通道区间值
#define SbusLow				-950
#define SbusHigh			950

// 驱动器ID
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


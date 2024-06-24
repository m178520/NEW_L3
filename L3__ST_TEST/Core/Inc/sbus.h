#ifndef __SBUS_H__
#define __SBUS_H__

#include "stdint.h"

/**
*控制模式
0 无控制 1 sbus控制 2 NAL控制
**/
#define noCont  	0 
#define sbusCont 	1
#define NALCont		2

#define SBUS_Data_Size 30 	// sbus数据长度 SBUSDATASIZE
#define Here_Median 1024 // 遥控器通道中间值

typedef struct
{
	uint8_t Start; //遥控器与接收机连接状态 0 未连接  1 已连接
	uint16_t CH1;
	uint16_t CH2;
	uint16_t CH3;
	uint16_t CH4;
	uint16_t CH5;
	uint16_t CH6;
	uint16_t CH7;
	uint16_t CH8;
	uint16_t CH9;
	uint16_t CH10;
	uint16_t CH11;
	uint16_t CH12;
	uint16_t CH13;
	uint16_t CH14;
	uint16_t CH15;
	uint16_t CH16;
	int16_t RSpeed;
	int16_t LSpeed;
	uint8_t Flag;
	uint8_t End;
}SBUS_t;

extern SBUS_t SBUS_CH;
extern uint8_t controlFlag;
extern uint8_t control_powerFlag;

void sbus_parse(uint8_t* bytes,uint8_t len);
float sbus_to_range(int16_t sbus_valude, float newMin, float newMax, float oldMin, float oldMax);
	
#endif

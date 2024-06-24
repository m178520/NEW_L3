#include "sbus.h"
#include "string.h"
#include "cmsis_os.h"

SBUS_t SBUS_CH = {0};
uint8_t controlFlag = noCont; 

uint8_t control_powerFlag = 0; //临时使用
uint8_t power_count = 1;

extern osSemaphoreId_t SBUS_RUN_SempHandle;

/*解析sbus的值*/
void sbus_parse(uint8_t* bytes,uint8_t len)
{
	char sbus_data[SBUS_Data_Size] = {0};

	memcpy(sbus_data,bytes,len);
	
	if(sbus_data[0] == 0x0F && sbus_data[24] == 0x00)
	{
		SBUS_CH.Start = 1;

		SBUS_CH.CH1 =  (sbus_data[ 1] >> 0 | (sbus_data[ 2] << 8 )) & 0x07FF;
		SBUS_CH.CH2 =  (sbus_data[ 2] >> 3 | (sbus_data[ 3] << 5 )) & 0x07FF;
		SBUS_CH.CH3 =  (sbus_data[ 3] >> 6 | (sbus_data[ 4] << 2 ) | sbus_data[ 5] << 10 ) & 0x07FF;
		SBUS_CH.CH4 =  (sbus_data[ 5] >> 1 | (sbus_data[ 6] << 7 )) & 0x07FF;
		SBUS_CH.CH5 =  (sbus_data[ 6] >> 4 | (sbus_data[ 7] << 4 )) & 0x07FF;
		SBUS_CH.CH6 =  (sbus_data[ 7] >> 7 | (sbus_data[ 8] << 1 ) | sbus_data[9] << 9 ) & 0x07FF;
		SBUS_CH.CH7 =  (sbus_data[ 9] >> 2 | (sbus_data[10] << 6 )) & 0x07FF;
		SBUS_CH.CH8 =  (sbus_data[10] >> 5 | (sbus_data[11] << 3 )) & 0x07FF;
		SBUS_CH.CH9 =  (sbus_data[12] << 0 | (sbus_data[13] << 8 )) & 0x07FF;
		SBUS_CH.CH10 = (sbus_data[13] >> 3 | (sbus_data[14] << 5 )) & 0x07FF;
		SBUS_CH.CH11 = (sbus_data[14] >> 6 | (sbus_data[15] << 2 ) | sbus_data[16] << 10 ) & 0x07FF;
		SBUS_CH.CH12 = (sbus_data[16] >> 1 | (sbus_data[17] << 7 )) & 0x07FF;
		SBUS_CH.CH13 = (sbus_data[17] >> 4 | (sbus_data[18] << 4 )) & 0x07FF;
		SBUS_CH.CH14 = (sbus_data[18] >> 7 | (sbus_data[19] << 1 ) | sbus_data[20] << 9 ) & 0x07FF;
		SBUS_CH.CH15 = (sbus_data[20] >> 2 | (sbus_data[21] << 6 )) & 0x07FF;
		SBUS_CH.CH16 = (sbus_data[21] >> 5 | (sbus_data[22] << 3 )) & 0x07FF;
		
		// herelink遥控器为通道1、2
		SBUS_CH.LSpeed = ((SBUS_CH.CH1 - Here_Median) - (SBUS_CH.CH2 - Here_Median));
		SBUS_CH.RSpeed = ((SBUS_CH.CH1 - Here_Median) + (SBUS_CH.CH2 - Here_Median));
		
		if(SBUS_CH.Start == 1 && SBUS_CH.CH6 == 201)
		{
			controlFlag = sbusCont;
			osSemaphoreRelease(SBUS_RUN_SempHandle);
		}
		else if(SBUS_CH.CH6 == 1801)
		{
			controlFlag = NALCont;
		}
		else if(controlFlag != NALCont)
		{
			controlFlag = noCont;
		}
		/*通道7暂时控制ec00u与GPS的通断*/    
		if(SBUS_CH.Start == 1 && SBUS_CH.CH7 == 201) //不可以上电
		{
			if(control_powerFlag == 1) power_count = 0;
			control_powerFlag = 0;
		}
		else if(SBUS_CH.CH7 == 1801) //可以上电
		{
			control_powerFlag = 1;
			power_count = 0;
		}
		
	}
}

/*将sbus信号转换为特定区间数值*/
float sbus_to_range(int16_t sbus_valude, float newMin, float newMax, float oldMin, float oldMax)
{
	float res = 0;//转化后的固定数值
	float newRange = 0;
	float oldRange = 0;
	float comp = 0;
	newRange = newMax - newMin;
	oldRange = oldMax - oldMin;
	comp = newRange / oldRange;
	res = comp * (sbus_valude - oldMin) + newMin;
	return res;
}


#include "motor.h"
#include "sbus.h"
#include "fdcan.h"

#include "stdint.h"
#include "string.h"

int8_t txSpeedMsg[8] = {0};

void driver_speed(int16_t speed, uint8_t driverID);

//void vcu_Trans_LGD(double Angle,double Speed)
//{
//	static uint8_t vcu_data[8] = {0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//	
//	/*对数组进行赋值*/
//	vcu_data[1] = Angle;
//	vcu_data[2] = Speed;
//	if(vcu_data[3] >= 255) vcu_data[3]++;
//	
//	usart2_send_data_apply(vcu_data,8);
//}

CAN_Msg_t Direct_Drive_motor(int16_t RSpeed, int16_t LSpeed)
{
	
	CAN_Msg_t L_R_Msg = {0};
		// 两侧轮子速度值
	int32_t RightSpeed = 0;
	int32_t LiftSpeed  = 0;
	if(controlFlag == NALCont)
	{	
		RightSpeed = -(int)sbus_to_range(RSpeed, NALNewMin, NALNewMax, NALOldMin, NALOldMax);
		LiftSpeed  = -(int)sbus_to_range(LSpeed, NALNewMin, NALNewMax, NALOldMin, NALOldMax);
		
		if(RightSpeed <= 30 && RightSpeed >= -30)
		{
			RightSpeed = CANSpeedStop;
		}
		else if(RightSpeed > CANSpeedHigh)
		{
			RightSpeed = CANSpeedHigh;
		}
		else if(RightSpeed < CANSpeedLow)
		{
			RightSpeed  = CANSpeedLow;
		}
		
		if(LiftSpeed <= 30 && LiftSpeed >= -30)
		{
			LiftSpeed = CANSpeedStop;
		}
		else if(LiftSpeed > CANSpeedHigh)
		{
			LiftSpeed = CANSpeedHigh;
		}
		else if(LiftSpeed < CANSpeedLow)
		{
			LiftSpeed  = CANSpeedLow;
		}
		
		driver_speed(LiftSpeed, ELECMAC_LIFT);
		memcpy(L_R_Msg.L_Msg,txSpeedMsg,8 );
		
		driver_speed(RightSpeed, ELECMAC_RIGHT);
		memcpy(L_R_Msg.R_Msg,txSpeedMsg,8 );
	}
	if(controlFlag == sbusCont)
	{
		RightSpeed = (int)sbus_to_range(RSpeed,  CANSpeedLow, CANSpeedHigh, SbusLow, SbusHigh);
		LiftSpeed  = -(int)sbus_to_range(LSpeed, CANSpeedLow, CANSpeedHigh, SbusLow, SbusHigh);
		
		if(RightSpeed < 50 && RightSpeed > -50)
		{
			RightSpeed = CANSpeedStop;
		}
		else if(RightSpeed >= CANSpeedHigh)
		{
			RightSpeed = CANSpeedHigh;
		}
		else if(RightSpeed <= CANSpeedLow)
		{
			RightSpeed = CANSpeedLow;
		}
		
		if(LiftSpeed < 50 && LiftSpeed > -50)
		{
			LiftSpeed = CANSpeedStop;
		}
		else if(LiftSpeed >= CANSpeedHigh)
		{
			LiftSpeed = CANSpeedHigh;
		}
		else if(LiftSpeed <= CANSpeedLow)
		{
			LiftSpeed = CANSpeedLow;
		}
		driver_speed(LiftSpeed, ELECMAC_LIFT);
		memcpy(L_R_Msg.L_Msg,txSpeedMsg,8 );
		
		driver_speed(RightSpeed, ELECMAC_RIGHT);
		memcpy(L_R_Msg.R_Msg,txSpeedMsg,8 );
			
	}
	else if(controlFlag == noCont)
	{
		RightSpeed = CANSpeedStop;
		LiftSpeed  = CANSpeedStop;
		
		driver_speed(LiftSpeed, ELECMAC_LIFT);
		memcpy(L_R_Msg.L_Msg,txSpeedMsg,8 );
		
		driver_speed(RightSpeed, ELECMAC_RIGHT);
		memcpy(L_R_Msg.R_Msg,txSpeedMsg,8 );
	}
	return L_R_Msg;
}

/* -------------------------------- begin  -------------------------------- */
/**
  * @Name    driver_speed
  * @brief  
	* @param   speed: 发送给驱动器的can速度值 
	**			 	 driverID: 电机ID
  * @retval
  * @author  zhs
  * @Data    2023-07-15
 **/
/* -------------------------------- end -------------------------------- */

void driver_speed(int16_t speed, uint8_t driverID)
{
	txSpeedMsg[0] = 0x23;
	txSpeedMsg[1] = 0x00;
	txSpeedMsg[2] = 0x20;
	txSpeedMsg[3] = driverID;
	txSpeedMsg[4] = speed & 0xff;
	txSpeedMsg[5] = (speed >> 8) & 0xff;
	txSpeedMsg[6] = 0x00;
	txSpeedMsg[7] = 0x00;
}

#ifndef _GPS_H
#define _GPS_H

#include "deal_string.h"


/*gpsœ˚œ¢*/
typedef struct{
	char    Lon[20];
	char    Lat[20];
	char    Altitude[10];
	char    Mode[2];
	char    UsedStateCnt[3];
	char    Hdop[10];
	char    CourseAngle[10];
	char    speed[10];
}GPS_t;

extern GPS_t gnss; 


//void USART_GPS_data (cJSON * object);
void SPI1_GPS_data(uint8_t *data);
	
#endif

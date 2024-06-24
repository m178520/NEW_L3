#ifndef _MQTT_H
#define _MQTT_H

#include "deal_string.h"
#include "rtk_L1.h"

#define MSG_SIZE 600

#define MQTT_APP_IP           "dev.server.sdxinzuobiao.com"
#define MQTT_APP_PORT         "1883"
#define MQTT_APP_NAME         "xzb"
#define MQTT_APP_PASSWORD     "sdXzb2023#"

/*MQTT信息*/
typedef struct{
	char ip[30];
	char port[10];
	char username[20];
	char password[20];
	char sub[100];
	char pub[100];
}MQTT_info_t;

/*MQTT消息结构体*/
typedef struct{
	int zoneId;
	int taskId;
}MQTT_Task_Msg_t;


extern int Knife_Height;
extern int MQTT_Return_Task_ZoneId;
extern int Safe_Zone_Detect;
extern MQTT_Task_Msg_t MQTT_Task_Msg;
extern char Change_Safe_Zone[100];
extern int  Light_Status;
extern int  Delete_Device;

extern MQTT_info_t MQTT_APP_info;

void USART_MQTT_data(cJSON * object);
void MQTT_open(char *fun,char *ip,char *port,char *username,char *password,char *sub);
void MQTT_pub(char* pub,char *msg);
void APP_Info_Submit(void);




#define MQTT_SUB_TOPIC(GROUPID,DEVICEID)  "xzb/navl3s/device/service/call/"GROUPID"/"DEVICEID""
#define MQTT_PUB_TOPIC(GROUPID,DEVICEID)  "xzb/autopilot/mower/device/property/post/"GROUPID"/"DEVICEID""
#define CONN_MSG(IP,PORT,USERNAME,PASSWORD,SUB)     "{\"IP\":\""IP"\",\"port\":\""PORT"\",\"username\":\""USERNAME"\",\"password\":\""PASSWORD"\",\"sub\":\""SUB"\"}"
#define PUB_MSG(PUB,MSG)                            "{\"pub\":\""PUB"\",\"msg\":"MSG"}"

#define DEVICE_MQTT_STATUS     "\
{\
\"property\":{\
\"obsFD\":100,\
\"status\": 1001,\
\"bleMac\": \"c8:c2:c6:5c:78:d7\",\
\"lat\": 36.831173850333,\
\"swath\": 1,\
\"obsSwitch\": 0,\
\"obstacle\": 100,\
\"RmotTemp\": 43,\
\"voltage\": 0,\
\"lon\": 118.077602868,\
\"current\": 0,\
\"battery\": 72,\
\"cutH\": 50,\
\"obstacler\": 100,\
\"time\": 1685090137,\
\"yaw\": -0.27799999999999,\
\"DmotTemp\": 60.12,\
\"netSignal\": 4,\
\"obsRD\": 50,\
\"obsLD\": 50,\
\"gId\": 51,\
\"safeS\": 1,\
\"obstaclel\": 100,\
\"dId\": 71,\
\"cId\": 1,\
\"HmotTemp\": 42,\
\"CmotTemp\": 42,\
\"LmotTemp\": 41,\
\"firmware\": \"1.0.32\",\
\"locSignal\": 5,\
\"light\": 0,\
\"cutS\": 1,\
\"speed\": \"0.4\"\
},\
\"task\": {\
\"pitch\": 0,\
\"OffCourse\": 0.0077417333048454,\
\"taskId\": 110,\
\"tStatus\": 1500,\
\"thisdis\": 0,\
\"yaw\": 0,\
\"tarang\": 0.97090281816185,\
\"thisang\": 0.91893546517247,\
\"roll\": 0,\
\"process\": 1,\
\"carang\": 0,\
\"tarIndex\": 1,\
\"zoneId\": 26,\
\"erdelang\": 1.2489028181618\
},\
\"healthy\": {\
\"network\": 3001,\
\"bound\": 3001,\
\"loc\": 3001,\
\"uplift\": 3001,\
\"battery\": 3001,\
\"tem\": 3001\
}\
}\
"
extern char Device_MQTT_Status[];
//#define DEVICE_MQTT_STATUS     "{\"property\":{\"obsFD\":100,\"status\":1001,\"bleMac\":\"c8:c2:c6:5c:78:d7\",\"lat\":36.831173850333,\"swath\":1,\"obsSwitch\":0,\"obstacle\":100,\"RmotTemp\":43,\"voltage\":0,\"lon\":118.077602868,\"current\":0,\"battery\":72,\"cutH\":50,\"obstacler\":100,\"time\":685090137,\"yaw\":-0.27799999999999,\"DmotTemp\":60.12,\"netSignal\":4,\"obsRD\":50,\"obsLD\":50,\"gId\":7,\"safeS\":1,\"obstaclel\":100,\"dId\":3,\"cId\":1,\"HmotTemp\":42,\"CmotTemp\":42,\"LmotTemp\":41,\"firmware\":\"1.0.32\",\"locSignal\":5,\"light\":0,\"cutS\":1,\"speed\":\"0.4\"},\"task\":{\"pitch\":0,\"OffCourse\":0.0077417333048454,\"taskId\":110,\"tStatus\":1500,\"thisdis\":0,\"yaw\":0,\"tarang\":0.97090281816185,\"thisang\":0.91893546517247,\"roll\":0,\"process\":1,\"carang\":0,\"tarIndex\":1,\"zoneId\":26,\"erdelang\":1.2489028181618},\"healthy\":{\"network\":3001,\"bound\":3001,\"loc\":3001,\"uplift\":3001,\"battery\":3001,\"tem\":3001}}"
#endif




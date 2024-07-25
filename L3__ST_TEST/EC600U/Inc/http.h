#ifndef _HTTP_H
#define _HTTP_H

#include "deal_string.h"
#include "mqtt.h"


#define BASE_URL        "http://dev.server.sdxinzuobiao.com:10006/embedded"
#define AUTHEN_URL      "http://dev.server.sdxinzuobiao.com:10006/embedded/authentication"
#define JOBSTART_URL    "http://dev.server.sdxinzuobiao.com:10006/embedded/jobStart"
#define JOBPAUSE_URL    "http://dev.server.sdxinzuobiao.com:10006/embedded/jobPause"
#define JOBCONTINUE_URL "http://dev.server.sdxinzuobiao.com:10006/embedded/jobContinue"
#define JOBFINISH_URL   "http://dev.server.sdxinzuobiao.com:10006/embedded/jobFinish"
#define UPDATEROUTE_URL "http://dev.server.sdxinzuobiao.com:10006/embedded/updateRoute"
#define CHARGE_URL      "http://dev.server.sdxinzuobiao.com:10006/embedded/goToCharge"


/*HTTP消息获取储存区*/
typedef struct{
	uint32_t groupId;
	uint32_t deviceId;
}Authen_info_t;

typedef struct{
	uint32_t zoneId;
	uint32_t taskId;
	char waypoints[500];
	uint32_t targetIndex;
	uint32_t offSet;
	uint32_t taskNum;
}HTTP_task_t;

typedef struct{
	uint32_t chargeId;
	char navWaypoints[150];
}Charge_info_t;

extern uint8_t request_num;

/*请求参数结构体*/
extern cJSON * EC600U_HTTP_jobPause;
extern cJSON * EC600U_HTTP_updateRoute;

extern Authen_info_t Authen_info;
extern HTTP_task_t   HTTP_Task_Msg ;
extern Charge_info_t Charge_info;

void HTTP_get(char* url,char* data,char *Header);
void HTTP_post(char* url,char* data,char *Header);
void HTTP_put(char* url,char* data,char *Header);

void USART_HTTP_data(cJSON * object);

/*串口HTTP数据来源函数*/
void USART_HTTP_Authen_data(cJSON * object);
void USART_HTTP_jobStart_data(cJSON * object);
void USART_HTTP_jobPause_data(cJSON * object);
void USART_HTTP_jobFinish_data(cJSON * object);
void USART_HTTP_jobContinue_data(cJSON * object);
void USART_HTTP_updateRoute_data(cJSON * object);
void USART_HTTP_goToCharge_data(cJSON * object);

/*HTTP请求消息*/
void HTTP_Authen_Request(void);
void HTTP_jobStart_Request(void);
void HTTP_jobPause_Request(void);
void HTTP_jobContinue_Request(void);
void HTTP_jobFinish_Request(void);
void HTTP_updateRoute_Request(void);
void HTTP_goToCharge_Request(void);

#define HTTP_REQUEST_MSG(URL,DATA,HEADER) "{\"url\":\""URL"\",\"data\":"DATA",\"Header\":"HEADER"}"
#define HTTP_REQUEST_HEADER_MSG(TYPE,DEVICEID,GROUPID) "{\"Content-Type\":\""TYPE"\",\"deviceId\":\""DEVICEID"\",\"groupId\":\""GROUPID"\"}"

#define UPDATE_ROUTE_PARAM "\
{\
  \"progress\": 0,\
  \"size\": 0,\
  \"startIndex\": 0,\
  \"tarIndex\": 0,\
	\"taskId\": 0,\
  \"zoneId\": 0\
}\
"

#define PAUSE_PARAM "\
{\
  \"pauseLat\": 0,\
  \"pauseLon\": 0,\
  \"progress\": 0,\
  \"targetIndex\": 0,\
  \"zoneId\": 0\
}\
"
extern char Update_Route_param[];
extern char Pause_param[];

#endif



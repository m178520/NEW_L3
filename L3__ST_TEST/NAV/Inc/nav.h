#ifndef __NAV_H__
#define __NAV_H__

#include "deal_string.h"
#include "WGS84ToAngle.h"

#define PI 3.14159265

#define CONTROL_WAY 1

#define VEHICLE_DIRECT_LEFT  -1   //������ת
#define VEHICLE_DIRECT_RIGHT 1    //������ת

#define FIXED_LENGHT 0.8          //�����н��복����L���ȵ�ֱ�ߵļн�

#define WAIT_RUN_POINT_NUM  50 
#define PROCESSED_POINT_NUM 30 

#define TERMINAL_RANGE      1     //��mΪ��λ

#define POINTTOLINE_THRES   1000  //�������뺽�߾��볬����ֵ �������

#define PID_ANGLE_THRES     1000  //pid����Ƕ�ֵ�������ֵ�����ڹ�һ��

extern char wait_run_point[WAIT_RUN_POINT_NUM][2][20];

typedef struct
{
	uint32_t processed_allnum;   //����ɴ��������ܸ���
	uint32_t Parse_num;          //���������ĵ��ܸ���
	uint32_t Parse_index;        //���������ĵ��ܸ���
	uint32_t current_toindex;    //��ǰ����ǰ���������
	uint32_t current_fromindex;  //��ǰǰ������������
}waypoints_run_status_t;

typedef struct
{
	int8_t direct;
	double value;
	Line_inter_point_t foot_point;
}tracking_control_t;

typedef struct
{
	double LSpeed;
	double RSpeed;
}NAV_output_t;

#if CONTROL_WAY == 0
typedef struct
{
	char   direct;
	double pointToline;
	double VehicleToStart;
	double StartToTerminal;
}pointToline_distance_t;



#else

typedef struct
{
	Line_straight_param_t origin_start_stop_line_param;
	WGS84_axis_t          origin_Vehicle_XY;
	WGS84_axis_t          Endpoint_XY;
	
	int8_t   direct;
	double pointToline;
	double StartToTerminal;
	double 		gnss_Angle;

}pointToline_distance_t;
#endif

extern char   HTTP_updateRoute_Request_flag;

extern waypoints_run_status_t waypoints_run_status;

void waypoints_Parse(char *string,char * str);
pointToline_distance_t pointToline_distance(double Vehicle_lat,double Vehicle_lon,double start_lat, double start_lon, double stop_lat, double stop_lon); //����ʹ��
NAV_output_t NAV_Control(void);

void NAV_Control_Param_clear(void);

#endif

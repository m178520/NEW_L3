#include "nav.h"
#include "gps.h"
#include "usart.h"
#include "pid.h"
#include "math.h"
#include "uart_data.h"
#include "motor.h"
#include "string.h"

#include "change_status.h"

char   HTTP_updateRoute_Request_flag = 0;       //用于将分段航线请求标志
static char Vehicle_To_Distance_Angle_flag = 0; //用于将起点与终点线在每次只计算一次

char wait_run_point[WAIT_RUN_POINT_NUM][2][20];

waypoints_run_status_t waypoints_run_status = {0};
extern cJSON * EC600U_MQTT_SEND_STATUS;

extern osMessageQueueId_t HTTP_REQUEST_queueHandle;

extern osEventFlagsId_t Device_Run_status_eventHandle;

PID_TypeDef PID_angle_control;

/*分割目前已获取的目标航点*/
void waypoints_Parse(char *string,char * str)
{
	char* q[20] = {0};
//	uint16_t num=0;
	int k = 0;

//	if(p[0] != 0 || p[7] != 0 || p[17] != 0)
//	{
//		while(p)
//		{
//			if(num != 0)  ++p;//第一次进入
//			num++;
//			p = strstr(p,str);
//		}
//	}
	
//	char* temp = strtok(string, str);
//	for(int i=0; temp != NULL; i++)
//	{
//		printf("%s\r\n", temp);
//		char *lon_lat = strtok(temp, " ");
//		for(int j=0; lon_lat != NULL ; j++)
//		{
//				printf("%s\r\n", lon_lat);
//				strcpy(wait_run_point[i + (waypoints_run_status.Parse_index/10)*10][j],lon_lat); //让每次都是从0 10 20 30 .....整十开始
//				q[k++] = lon_lat;
//				if (j >= 2)  break;
//				lon_lat = strtok(NULL, " ");
//		}
//		if (i >= 10)  break;
//		waypoints_run_status.Parse_index++;
//		if(waypoints_run_status.Parse_index >= 50) waypoints_run_status.Parse_index = 0;
//		waypoints_run_status.Parse_num  ++;
//		if(strstr(temp,",")) temp = strtok(temp+strlen(q[k-1]) + strlen(q[k-2])+2, str);
//	}
		uint16_t lenght = strlen(string);
	  char *p = string;                  //初始位置
		char* temp = strstr(p, str);  //找到，的位置
		*temp = 0;
		
		for(int i=0; *temp == 0 || temp == NULL ; i++)
		{
			char *lon_lat = strtok(p, " ");
			for(int j=0; lon_lat != NULL ; j++)
			{
//					printf("%s\r\n", lon_lat);
					strcpy(wait_run_point[i + (waypoints_run_status.Parse_index/10)*10][j],lon_lat); //让每次都是从0 10 20 30 .....整十开始
					q[k++] = lon_lat;
					if (j >= 2)  break;
					lon_lat = strtok(NULL, " ");
			}
			
			waypoints_run_status.Parse_index++;
			if(waypoints_run_status.Parse_index >= 50) waypoints_run_status.Parse_index = 0;
			waypoints_run_status.Parse_num  ++;
			
			p += strlen(q[k-1]) + strlen(q[k-2])+2;
			if(p >= string + lenght) break;

			temp = strstr(p, str);  //找到，的位置
			if(temp != NULL) *temp = 0;
			else						 continue;
		}

}


/*求车辆点至目标航线垂直距离与在线的哪一侧*/
pointToline_distance_t pointToline_distance(double Vehicle_lat,double Vehicle_lon,double start_lat, double start_lon, double stop_lat, double stop_lon)
{
	double Angle = 0;
	pointToline_distance_t pointToline_info = {0};
	static WGS84_axis_t Endpoint_XY = {0};
	static Line_straight_param_t start_stop_line = {0};
	
	/*如果前往的终点不正确，终点为（0，0）停止车辆的导航*/
	if(stop_lat == 0 ||stop_lon == 0)  
	{
		if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_23) != 0)
		osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
	}
	
	if(Vehicle_To_Distance_Angle_flag == 0) //本次航线初次进入计算
	{
		
//		printf("第%d次,,%.10f,%.10f,%.10f,%.10f\r\n", waypoints_run_status.current_toindex , start_lat,  start_lon,  stop_lat,  stop_lon);
		/*求以起点为原点，终点的坐标*/
		Endpoint_XY = GPStoXY(start_lat,start_lon,stop_lat,stop_lon);
		
		/*求出函数系数*/
		start_stop_line = line_fun(0,0,Endpoint_XY.x,Endpoint_XY.y);
		
		Vehicle_To_Distance_Angle_flag ++;
	}
	/*求以起点为原点，车辆的坐标*/
	WGS84_axis_t Vehicle_XY  = GPStoXY(Endpoint_XY.origin_lat,Endpoint_XY.origin_lon,Vehicle_lat,Vehicle_lon);
	
	double gnss_Angle = strtod(gnss.CourseAngle,NULL);

	
		
	if(Device_Run_Status.Curstatus == Job_Return && waypoints_run_status.processed_allnum >= 1 ) //说明在返航中到达了第一个点，需要调转车头，将车尾作为车头进行行走，左右轮需要调换，RTK导航方向需要掉头
	{
		/*在倒车时不需要将车体坐标系平移*/
		/*但是需要将rtk方向反转180*/
		if(gnss_Angle >= 180)   gnss_Angle -= 180;
		else                    gnss_Angle += 180;
		if(gnss_Angle > 180)
		{
			gnss_Angle = gnss_Angle - 360.0;
		}
	}
	else
	{
		if(gnss_Angle > 180)
		{
			gnss_Angle = gnss_Angle - 360.0;
		}
		/* 转换坐标系为车头坐标系 */
		//将gnss方向坐标以北为0转换至以东为0
		if(gnss_Angle < -90)
		{
			Angle =-180 - (gnss_Angle+90);
		}
		else
		{
			Angle = - (gnss_Angle-90);
		}
		//将车坐标系作为车辆的点，下面两句是以后接收机位置为基点进行平移坐标系
		Vehicle_XY.x += 1.2 * cos(Angle * PI / 180);
		Vehicle_XY.y += 1.2 * sin(Angle * PI / 180);
	}
	
	pointToline_info.gnss_Angle = gnss_Angle;
	
	#if CONTROL_WAY == 1
	pointToline_info.origin_start_stop_line_param = start_stop_line;
	pointToline_info.origin_Vehicle_XY            = Vehicle_XY;
	pointToline_info.Endpoint_XY                  = Endpoint_XY;
	
	#endif
	
	pointToline_info.StartToTerminal = Endpoint_XY.length;
	
	/*计算点至线的距离*/              
	double denominator = sqrt(1 + start_stop_line.k * start_stop_line.k);                
	double numerator = fabs(start_stop_line.k * Vehicle_XY.x - Vehicle_XY.y + start_stop_line.b);              
	pointToline_info.pointToline = fabs(numerator / denominator);

//	/*判断方向*/
//	double pose_direction = (Vehicle_XY.y - Endpoint_XY.y) / (Vehicle_XY.x - Endpoint_XY.x);
//	double direction_err = pose_direction - start_stop_line.k;
//	
//	if (direction_err < 0)      pointToline_info.direct = VEHICLE_DIRECT_RIGHT;
//	else                        pointToline_info.direct = VEHICLE_DIRECT_LEFT;

	return pointToline_info;
}


/*轨迹跟踪控制*/
/*只是横向的控制*/
tracking_control_t tracking_control_Arith(PID_TypeDef *PID_InitStruct,pointToline_distance_t info)
{
	double lenght = FIXED_LENGHT;
	double atan_value = 0;
	if(info.pointToline < 0.5) lenght *=2;
	tracking_control_t tracking_control = {0};
	#if CONTROL_WAY == 0 //直接拐 在线的左边就向右拐 距离作为拐弯的大小值
		/*偏离航信过大，就打满方向*/
		if(info.pointToline > POINTTOLINE_THRES  )
		{
			tracking_control.value = 1;
		}
		/*偏离航线不大，使用点至线的距离求解个方向量*/
		else
		{
			/*将线与点之间的距离输入pid求取转向角控制量*/
			double PID_Angle = PID_Update(PID_InitStruct,info.pointToline,0);
			/*进行归一化*/
//			tracking_control.value = PID_Angle/PID_ANGLE_THRES;
			tracking_control.value = PID_Angle;
		}
		tracking_control.direct = info.direct;
	#else //画一个三角形，不断跟踪三角形的角
	double x,y;
	double target_degree;
	Line_straight_param_t target_line = {0};
		/*将车辆点至直线的垂足坐标求出*/
		/*将过车辆点的垂线求出*/
		Line_straight_param_t vertical_line_fun_param = vertical_line_fun(info.origin_Vehicle_XY.x,info.origin_Vehicle_XY.y,info.origin_start_stop_line_param.k,info.origin_start_stop_line_param.b);
		/*求出垂足*/	
		Line_inter_point_t    Line_inter_point        = Line_inter_point_math(info.origin_start_stop_line_param.k,info.origin_start_stop_line_param.b,vertical_line_fun_param.k,vertical_line_fun_param.b);

		tracking_control.foot_point = Line_inter_point;
		
		/*将垂足至三角形边长为一定值的坐标求出*/
	
	if(info.origin_start_stop_line_param.k == inf)            atan_value = 90 * PI / 180;
	else if (info.origin_start_stop_line_param.k == -inf)			atan_value = -(90 * PI / 180);
	else                                                      atan_value = atan(info.origin_start_stop_line_param.k);
		if(info.Endpoint_XY.x >=0)
		{
			y = Line_inter_point.y + lenght * sin(atan_value);
			x = Line_inter_point.x + lenght * cos(atan_value);
		}
		else
		{
			y = Line_inter_point.y - lenght * sin(atan_value);
			x = Line_inter_point.x - lenght * cos(atan_value);
		}
		/*求出直线的角度*/
		target_line = line_fun(info.origin_Vehicle_XY.x,info.origin_Vehicle_XY.y,x,y);
		
		if(target_line.k == inf)     															atan_value = 90 * PI / 180;
		else if(target_line.k == -inf)														atan_value = -(90 * PI / 180);
		else                                                      atan_value = atan(target_line.k);
		double degree = atan_value * 180.0 / PI;
		
		if(y-info.origin_Vehicle_XY.y >= 0 && x - info.origin_Vehicle_XY.x < 0) //如果位于第二象限
		{
				target_degree = 180.0 + degree;
		}
		else if(y-info.origin_Vehicle_XY.y < 0 && x - info.origin_Vehicle_XY.x < 0) //如果位于第三象限
		{
				target_degree =  degree - 180.0 ;
		}
		else //位于一四象限
		{
			target_degree  = degree;
		}
		
		/*将x坐标轴为0度变换为将y周正方向为0度*/
		target_degree = -1.0 * target_degree +90;
		

		double err = info.gnss_Angle - target_degree;
		double fabs_err = fabs(err);
		
		tracking_control.value = fabs_err ;  //归一化
		
		/*确定方向*/
		if(err > 0 )                tracking_control.direct = VEHICLE_DIRECT_LEFT;
		else if(err < 0)            tracking_control.direct = VEHICLE_DIRECT_RIGHT;
		else                        tracking_control.direct = 0;
		
		
		if(fabs_err > 180)  
		{
			tracking_control.direct =  -tracking_control.direct;
			tracking_control.value  =  360 - fabs_err; /*0-180*/
		}
		
	#endif
	return tracking_control;
}


/*终点判定函数*/
char  Terminal_decision(double distance)
{

	if(distance <= TERMINAL_RANGE)  return 0; //说明到达终点
	else                            return 1; //未到达终点
}

NAV_output_t NAV_Control()
{
	uint32_t BIT=0;
	NAV_output_t NAV_output;
	double Lenght,process = 0;
	double Angle,Speed = 120;
	pointToline_distance_t pointToline_info = {0};
	tracking_control_t     tracking_control = {0};
	if(waypoints_run_status.processed_allnum != 0) //不是前往初始点
	{
		/*求车辆点至目标航线垂直距离与方向*/
		pointToline_info	= pointToline_distance(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_fromindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_fromindex][1],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL));
	}
	else //前往初始点
	{
		/*求车辆点至目标航线垂直距离与方向*/
		pointToline_info	= pointToline_distance(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL));
	}
	/*执行终点判定*/
	/*通过判断是否到达终点计算*/
//	if(Terminal_decision(GPStoXY(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL)).length))/*未到达终点*/
	if(Terminal_decision(sqrt((pointToline_info.origin_Vehicle_XY.x-pointToline_info.Endpoint_XY.x) * (pointToline_info.origin_Vehicle_XY.x-pointToline_info.Endpoint_XY.x) + (pointToline_info.origin_Vehicle_XY.y-pointToline_info.Endpoint_XY.y) * (pointToline_info.origin_Vehicle_XY.y-pointToline_info.Endpoint_XY.y))))
{
		/*轨迹跟踪计算*/
		tracking_control = tracking_control_Arith(&PID_angle_control,pointToline_info);
		
		
		/*如果车辆到航线距离小于10cm=0.1m，则认为在线上*/
			/*格式转换*/
			if(tracking_control.direct == VEHICLE_DIRECT_LEFT)              Angle =  tracking_control.value ;    //方向值0~90
			else if(tracking_control.direct == VEHICLE_DIRECT_RIGHT)        Angle = -tracking_control.value ;
			else                                                            Angle = 0 ;
		
		/*驱动控制*/
		//Speed目前手动给值
		if(Device_Run_Status.Curstatus == Job_Return && waypoints_run_status.processed_allnum >= 1 ) //说明在返航中到达了第一个点，需要调转车头，将车尾作为车头进行行走，左右轮需要调换，RTK导航方向需要掉头
		{
			NAV_output.RSpeed =  Speed - Angle;
			NAV_output.LSpeed =  Speed + Angle;
		}
		else
		{
			NAV_output.RSpeed =  Speed + Angle;
			NAV_output.LSpeed =  Speed - Angle;
		}

		/*进度计算*/
	  /*通过是否到达指定长度计算*/
		/*求出垂足与起点的距离用于进度计算*/
		if((tracking_control.foot_point.x * pointToline_info.Endpoint_XY.x)>= 0 && (tracking_control.foot_point.y * pointToline_info.Endpoint_XY.y)>=0) //说明垂足与终点在一个象限
		{
			Lenght = sqrt(tracking_control.foot_point.x  * tracking_control.foot_point.x + tracking_control.foot_point.y  * tracking_control.foot_point.y );
		}
		else
		{
			Lenght = 0;
		}
			process = Lenght / pointToline_info.StartToTerminal;
	
		if(process >= 1)//说明到达终点
		{
			if(waypoints_run_status.current_toindex > 0)
			{
				if(++waypoints_run_status.current_toindex >= 50)  waypoints_run_status.current_toindex =0;
				if(++waypoints_run_status.current_fromindex>= 50) waypoints_run_status.current_fromindex =0; 
			}
			else
			{
				if(waypoints_run_status.Parse_num > 10)
				{
					++waypoints_run_status.current_toindex  ;
					waypoints_run_status.current_fromindex = 0;
				}
				else waypoints_run_status.current_toindex++;
			}
			
			if(process > 1) //进度赋值为1
			{
				process = 1;
			}
			
			Vehicle_To_Distance_Angle_flag = 0;
			HTTP_updateRoute_Request_flag  = 0; //获取分段航点之后需要进入终点后再置位，不然会获取多次
			waypoints_run_status.processed_allnum ++;
			
		}
	}
	else /*到达终点*/
	{
		if(waypoints_run_status.current_toindex > 0)
		{
			if(++waypoints_run_status.current_toindex >= 50)  waypoints_run_status.current_toindex =0;
			if(++waypoints_run_status.current_fromindex>= 50) waypoints_run_status.current_fromindex =0; 
		}
		else
		{
			if(waypoints_run_status.Parse_num > 10)
			{
				++waypoints_run_status.current_toindex;
			  waypoints_run_status.current_fromindex = 0;
			}
			else waypoints_run_status.current_toindex++;
		}			

		Vehicle_To_Distance_Angle_flag = 0;
		HTTP_updateRoute_Request_flag  = 0; //获取分段航点之后需要进入终点后再置位，不然会获取多次
		
		waypoints_run_status.processed_allnum ++;
		
//		/*进入临界区*/
//			taskENTER_CRITICAL();
//			Json_data_Change(EC600U_MQTT_SEND_STATUS,"%f%s%s",process,"task","process");
//			/*退出临界区*/
//			taskEXIT_CRITICAL();
	}
	if(waypoints_run_status.Parse_num >= HTTP_Task_Msg.taskNum)
	{
		if(waypoints_run_status.processed_allnum == HTTP_Task_Msg.taskNum)  //说明完成了所有航线
		{
			if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_23) != 0)
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
			if(HTTP_updateRoute_Request_flag == 0)                            //d终点后执行一次完成任务HTTP请求
			{
				HTTP_updateRoute_Request_flag++;
				BIT = BIT_4;
				osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);//交给HTTP进行任务完成请求
			}
		}
	}
	else
	{
		/*每次从服务器哪里获取10个数据点 每次车辆到达本次航线取点数的第7位时去获取剩余的点*/
		if(waypoints_run_status.processed_allnum % 10 == 7 && HTTP_updateRoute_Request_flag == 0) 
		{
			HTTP_updateRoute_Request_flag++;
			BIT = BIT_5;
			osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);//交给HTTP进行分段航线请求
		}
	}

	
//	printf("方向量：%f,方向：%d\r\n",Angle,tracking_control.direct);
	return NAV_output;
}

void NAV_Control_Param_clear()
{
	Vehicle_To_Distance_Angle_flag = 0;

	memset(&waypoints_run_status,0,sizeof(waypoints_run_status));
	memset(wait_run_point,0,sizeof(wait_run_point));
}


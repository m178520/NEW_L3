#include "nav.h"
#include "gps.h"
#include "usart.h"
#include "pid.h"
#include "math.h"
#include "uart_data.h"
#include "motor.h"
#include "string.h"

#include "change_status.h"

char   HTTP_updateRoute_Request_flag = 0;       //���ڽ��ֶκ��������־
static char Vehicle_To_Distance_Angle_flag = 0; //���ڽ�������յ�����ÿ��ֻ����һ��

char wait_run_point[WAIT_RUN_POINT_NUM][2][20];

waypoints_run_status_t waypoints_run_status = {0};
extern cJSON * EC600U_MQTT_SEND_STATUS;

extern osMessageQueueId_t HTTP_REQUEST_queueHandle;

extern osEventFlagsId_t Device_Run_status_eventHandle;

PID_TypeDef PID_angle_control;

/*�ָ�Ŀǰ�ѻ�ȡ��Ŀ�꺽��*/
void waypoints_Parse(char *string,char * str)
{
	char* q[20] = {0};
//	uint16_t num=0;
	int k = 0;

//	if(p[0] != 0 || p[7] != 0 || p[17] != 0)
//	{
//		while(p)
//		{
//			if(num != 0)  ++p;//��һ�ν���
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
//				strcpy(wait_run_point[i + (waypoints_run_status.Parse_index/10)*10][j],lon_lat); //��ÿ�ζ��Ǵ�0 10 20 30 .....��ʮ��ʼ
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
	  char *p = string;                  //��ʼλ��
		char* temp = strstr(p, str);  //�ҵ�����λ��
		*temp = 0;
		
		for(int i=0; *temp == 0 || temp == NULL ; i++)
		{
			char *lon_lat = strtok(p, " ");
			for(int j=0; lon_lat != NULL ; j++)
			{
//					printf("%s\r\n", lon_lat);
					strcpy(wait_run_point[i + (waypoints_run_status.Parse_index/10)*10][j],lon_lat); //��ÿ�ζ��Ǵ�0 10 20 30 .....��ʮ��ʼ
					q[k++] = lon_lat;
					if (j >= 2)  break;
					lon_lat = strtok(NULL, " ");
			}
			
			waypoints_run_status.Parse_index++;
			if(waypoints_run_status.Parse_index >= 50) waypoints_run_status.Parse_index = 0;
			waypoints_run_status.Parse_num  ++;
			
			p += strlen(q[k-1]) + strlen(q[k-2])+2;
			if(p >= string + lenght) break;

			temp = strstr(p, str);  //�ҵ�����λ��
			if(temp != NULL) *temp = 0;
			else						 continue;
		}

}


/*��������Ŀ�꺽�ߴ�ֱ���������ߵ���һ��*/
pointToline_distance_t pointToline_distance(double Vehicle_lat,double Vehicle_lon,double start_lat, double start_lon, double stop_lat, double stop_lon)
{
	double Angle = 0;
	pointToline_distance_t pointToline_info = {0};
	static WGS84_axis_t Endpoint_XY = {0};
	static Line_straight_param_t start_stop_line = {0};
	
	/*���ǰ�����յ㲻��ȷ���յ�Ϊ��0��0��ֹͣ�����ĵ���*/
	if(stop_lat == 0 ||stop_lon == 0)  
	{
		if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_23) != 0)
		osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
	}
	
	if(Vehicle_To_Distance_Angle_flag == 0) //���κ��߳��ν������
	{
		
//		printf("��%d��,,%.10f,%.10f,%.10f,%.10f\r\n", waypoints_run_status.current_toindex , start_lat,  start_lon,  stop_lat,  stop_lon);
		/*�������Ϊԭ�㣬�յ������*/
		Endpoint_XY = GPStoXY(start_lat,start_lon,stop_lat,stop_lon);
		
		/*�������ϵ��*/
		start_stop_line = line_fun(0,0,Endpoint_XY.x,Endpoint_XY.y);
		
		Vehicle_To_Distance_Angle_flag ++;
	}
	/*�������Ϊԭ�㣬����������*/
	WGS84_axis_t Vehicle_XY  = GPStoXY(Endpoint_XY.origin_lat,Endpoint_XY.origin_lon,Vehicle_lat,Vehicle_lon);
	
	double gnss_Angle = strtod(gnss.CourseAngle,NULL);

	
		
	if(Device_Run_Status.Curstatus == Job_Return && waypoints_run_status.processed_allnum >= 1 ) //˵���ڷ����е����˵�һ���㣬��Ҫ��ת��ͷ������β��Ϊ��ͷ�������ߣ���������Ҫ������RTK����������Ҫ��ͷ
	{
		/*�ڵ���ʱ����Ҫ����������ϵƽ��*/
		/*������Ҫ��rtk����ת180*/
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
		/* ת������ϵΪ��ͷ����ϵ */
		//��gnss���������Ա�Ϊ0ת�����Զ�Ϊ0
		if(gnss_Angle < -90)
		{
			Angle =-180 - (gnss_Angle+90);
		}
		else
		{
			Angle = - (gnss_Angle-90);
		}
		//��������ϵ��Ϊ�����ĵ㣬�����������Ժ���ջ�λ��Ϊ�������ƽ������ϵ
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
	
	/*��������ߵľ���*/              
	double denominator = sqrt(1 + start_stop_line.k * start_stop_line.k);                
	double numerator = fabs(start_stop_line.k * Vehicle_XY.x - Vehicle_XY.y + start_stop_line.b);              
	pointToline_info.pointToline = fabs(numerator / denominator);

//	/*�жϷ���*/
//	double pose_direction = (Vehicle_XY.y - Endpoint_XY.y) / (Vehicle_XY.x - Endpoint_XY.x);
//	double direction_err = pose_direction - start_stop_line.k;
//	
//	if (direction_err < 0)      pointToline_info.direct = VEHICLE_DIRECT_RIGHT;
//	else                        pointToline_info.direct = VEHICLE_DIRECT_LEFT;

	return pointToline_info;
}


/*�켣���ٿ���*/
/*ֻ�Ǻ���Ŀ���*/
tracking_control_t tracking_control_Arith(PID_TypeDef *PID_InitStruct,pointToline_distance_t info)
{
	double lenght = FIXED_LENGHT;
	double atan_value = 0;
	if(info.pointToline < 0.5) lenght *=2;
	tracking_control_t tracking_control = {0};
	#if CONTROL_WAY == 0 //ֱ�ӹ� ���ߵ���߾����ҹ� ������Ϊ����Ĵ�Сֵ
		/*ƫ�뺽�Ź��󣬾ʹ�������*/
		if(info.pointToline > POINTTOLINE_THRES  )
		{
			tracking_control.value = 1;
		}
		/*ƫ�뺽�߲���ʹ�õ����ߵľ�������������*/
		else
		{
			/*�������֮��ľ�������pid��ȡת��ǿ�����*/
			double PID_Angle = PID_Update(PID_InitStruct,info.pointToline,0);
			/*���й�һ��*/
//			tracking_control.value = PID_Angle/PID_ANGLE_THRES;
			tracking_control.value = PID_Angle;
		}
		tracking_control.direct = info.direct;
	#else //��һ�������Σ����ϸ��������εĽ�
	double x,y;
	double target_degree;
	Line_straight_param_t target_line = {0};
		/*����������ֱ�ߵĴ����������*/
		/*����������Ĵ������*/
		Line_straight_param_t vertical_line_fun_param = vertical_line_fun(info.origin_Vehicle_XY.x,info.origin_Vehicle_XY.y,info.origin_start_stop_line_param.k,info.origin_start_stop_line_param.b);
		/*�������*/	
		Line_inter_point_t    Line_inter_point        = Line_inter_point_math(info.origin_start_stop_line_param.k,info.origin_start_stop_line_param.b,vertical_line_fun_param.k,vertical_line_fun_param.b);

		tracking_control.foot_point = Line_inter_point;
		
		/*�������������α߳�Ϊһ��ֵ���������*/
	
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
		/*���ֱ�ߵĽǶ�*/
		target_line = line_fun(info.origin_Vehicle_XY.x,info.origin_Vehicle_XY.y,x,y);
		
		if(target_line.k == inf)     															atan_value = 90 * PI / 180;
		else if(target_line.k == -inf)														atan_value = -(90 * PI / 180);
		else                                                      atan_value = atan(target_line.k);
		double degree = atan_value * 180.0 / PI;
		
		if(y-info.origin_Vehicle_XY.y >= 0 && x - info.origin_Vehicle_XY.x < 0) //���λ�ڵڶ�����
		{
				target_degree = 180.0 + degree;
		}
		else if(y-info.origin_Vehicle_XY.y < 0 && x - info.origin_Vehicle_XY.x < 0) //���λ�ڵ�������
		{
				target_degree =  degree - 180.0 ;
		}
		else //λ��һ������
		{
			target_degree  = degree;
		}
		
		/*��x������Ϊ0�ȱ任Ϊ��y��������Ϊ0��*/
		target_degree = -1.0 * target_degree +90;
		

		double err = info.gnss_Angle - target_degree;
		double fabs_err = fabs(err);
		
		tracking_control.value = fabs_err ;  //��һ��
		
		/*ȷ������*/
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


/*�յ��ж�����*/
char  Terminal_decision(double distance)
{

	if(distance <= TERMINAL_RANGE)  return 0; //˵�������յ�
	else                            return 1; //δ�����յ�
}

NAV_output_t NAV_Control()
{
	uint32_t BIT=0;
	NAV_output_t NAV_output;
	double Lenght,process = 0;
	double Angle,Speed = 120;
	pointToline_distance_t pointToline_info = {0};
	tracking_control_t     tracking_control = {0};
	if(waypoints_run_status.processed_allnum != 0) //����ǰ����ʼ��
	{
		/*��������Ŀ�꺽�ߴ�ֱ�����뷽��*/
		pointToline_info	= pointToline_distance(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_fromindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_fromindex][1],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL));
	}
	else //ǰ����ʼ��
	{
		/*��������Ŀ�꺽�ߴ�ֱ�����뷽��*/
		pointToline_info	= pointToline_distance(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL));
	}
	/*ִ���յ��ж�*/
	/*ͨ���ж��Ƿ񵽴��յ����*/
//	if(Terminal_decision(GPStoXY(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL)).length))/*δ�����յ�*/
	if(Terminal_decision(sqrt((pointToline_info.origin_Vehicle_XY.x-pointToline_info.Endpoint_XY.x) * (pointToline_info.origin_Vehicle_XY.x-pointToline_info.Endpoint_XY.x) + (pointToline_info.origin_Vehicle_XY.y-pointToline_info.Endpoint_XY.y) * (pointToline_info.origin_Vehicle_XY.y-pointToline_info.Endpoint_XY.y))))
{
		/*�켣���ټ���*/
		tracking_control = tracking_control_Arith(&PID_angle_control,pointToline_info);
		
		
		/*������������߾���С��10cm=0.1m������Ϊ������*/
			/*��ʽת��*/
			if(tracking_control.direct == VEHICLE_DIRECT_LEFT)              Angle =  tracking_control.value ;    //����ֵ0~90
			else if(tracking_control.direct == VEHICLE_DIRECT_RIGHT)        Angle = -tracking_control.value ;
			else                                                            Angle = 0 ;
		
		/*��������*/
		//SpeedĿǰ�ֶ���ֵ
		if(Device_Run_Status.Curstatus == Job_Return && waypoints_run_status.processed_allnum >= 1 ) //˵���ڷ����е����˵�һ���㣬��Ҫ��ת��ͷ������β��Ϊ��ͷ�������ߣ���������Ҫ������RTK����������Ҫ��ͷ
		{
			NAV_output.RSpeed =  Speed - Angle;
			NAV_output.LSpeed =  Speed + Angle;
		}
		else
		{
			NAV_output.RSpeed =  Speed + Angle;
			NAV_output.LSpeed =  Speed - Angle;
		}

		/*���ȼ���*/
	  /*ͨ���Ƿ񵽴�ָ�����ȼ���*/
		/*������������ľ������ڽ��ȼ���*/
		if((tracking_control.foot_point.x * pointToline_info.Endpoint_XY.x)>= 0 && (tracking_control.foot_point.y * pointToline_info.Endpoint_XY.y)>=0) //˵���������յ���һ������
		{
			Lenght = sqrt(tracking_control.foot_point.x  * tracking_control.foot_point.x + tracking_control.foot_point.y  * tracking_control.foot_point.y );
		}
		else
		{
			Lenght = 0;
		}
			process = Lenght / pointToline_info.StartToTerminal;
	
		if(process >= 1)//˵�������յ�
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
			
			if(process > 1) //���ȸ�ֵΪ1
			{
				process = 1;
			}
			
			Vehicle_To_Distance_Angle_flag = 0;
			HTTP_updateRoute_Request_flag  = 0; //��ȡ�ֶκ���֮����Ҫ�����յ������λ����Ȼ���ȡ���
			waypoints_run_status.processed_allnum ++;
			
		}
	}
	else /*�����յ�*/
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
		HTTP_updateRoute_Request_flag  = 0; //��ȡ�ֶκ���֮����Ҫ�����յ������λ����Ȼ���ȡ���
		
		waypoints_run_status.processed_allnum ++;
		
//		/*�����ٽ���*/
//			taskENTER_CRITICAL();
//			Json_data_Change(EC600U_MQTT_SEND_STATUS,"%f%s%s",process,"task","process");
//			/*�˳��ٽ���*/
//			taskEXIT_CRITICAL();
	}
	if(waypoints_run_status.Parse_num >= HTTP_Task_Msg.taskNum)
	{
		if(waypoints_run_status.processed_allnum == HTTP_Task_Msg.taskNum)  //˵����������к���
		{
			if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_23) != 0)
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
			if(HTTP_updateRoute_Request_flag == 0)                            //d�յ��ִ��һ���������HTTP����
			{
				HTTP_updateRoute_Request_flag++;
				BIT = BIT_4;
				osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);//����HTTP���������������
			}
		}
	}
	else
	{
		/*ÿ�δӷ����������ȡ10�����ݵ� ÿ�γ������ﱾ�κ���ȡ�����ĵ�7λʱȥ��ȡʣ��ĵ�*/
		if(waypoints_run_status.processed_allnum % 10 == 7 && HTTP_updateRoute_Request_flag == 0) 
		{
			HTTP_updateRoute_Request_flag++;
			BIT = BIT_5;
			osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);//����HTTP���зֶκ�������
		}
	}

	
//	printf("��������%f,����%d\r\n",Angle,tracking_control.direct);
	return NAV_output;
}

void NAV_Control_Param_clear()
{
	Vehicle_To_Distance_Angle_flag = 0;

	memset(&waypoints_run_status,0,sizeof(waypoints_run_status));
	memset(wait_run_point,0,sizeof(wait_run_point));
}


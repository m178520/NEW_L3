#include "nav.h"
#include "gps.h"
#include "usart.h"
#include "pid.h"
#include "math.h"
#include "uart_data.h"
#include "motor.h"

static char Vehicle_To_Distance_Angle_flag = 0; //���ڽ�������յ�����ÿ��ֻ����һ��
char wait_run_point[WAIT_RUN_POINT_NUM][2][20];

waypoints_run_status_t waypoints_run_status = {0};


PID_TypeDef PID_angle_control;

/*�ָ�Ŀǰ�ѻ�ȡ��Ŀ�꺽��*/
void waypoints_Parse(char *string,char * str)
{
	char* q[20] = {0};
	char* temp = strtok(string, str);
	int k = 0;
	for(int i=0; temp != NULL; i++)
	{
//		printf("%s\r\n", temp);
		char *lon_lat = strtok(temp, " ");
		for(int j=0; lon_lat != NULL ; j++)
		{
//				printf("%s\r\n", lon_lat);
				strcpy(wait_run_point[i + (waypoints_run_status.Parse_index/10)*10][j],lon_lat);
				q[k++] = lon_lat;
				if (j >= 2)  break;
				lon_lat = strtok(NULL, " ");
		}
		waypoints_run_status.Parse_index++;
		if(waypoints_run_status.Parse_index >= 50) waypoints_run_status.Parse_index = 0;
		waypoints_run_status.Parse_num  ++;
		if (i >= 10)  break;
		temp = strtok(temp+strlen(q[k-1]) + strlen(q[k-2])+2, str); 
	}
}


/*��������Ŀ�꺽�ߴ�ֱ���������ߵ���һ��*/
pointToline_distance_t pointToline_distance(double Vehicle_lat,double Vehicle_lon,double start_lat, double start_lon, double stop_lat, double stop_lon)
{
	pointToline_distance_t pointToline_info = {0};
	static WGS84_axis_t Endpoint_XY = {0};
	static Line_straight_param_t start_stop_line = {0};
	if(Vehicle_To_Distance_Angle_flag == 0) //���κ��߳��ν������
	{
		/*�������Ϊԭ�㣬�յ������*/
		Endpoint_XY = GPStoXY(start_lat,start_lon,stop_lat,stop_lon);
		
		/*�������ϵ��*/
		start_stop_line = line_fun(0,0,Endpoint_XY.x,Endpoint_XY.y);
		
		Vehicle_To_Distance_Angle_flag ++;
	}
	/*�������Ϊԭ�㣬����������*/
	WGS84_axis_t Vehicle_XY  = GPStoXY(Endpoint_XY.origin_lat,Endpoint_XY.origin_lon,Vehicle_lat,Vehicle_lon);
	
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
		if(info.Endpoint_XY.x >=0)
		{
			y = Line_inter_point.y + FIXED_LENGHT * sin(atan(info.origin_start_stop_line_param.k));
			x = Line_inter_point.x + FIXED_LENGHT * cos(atan(info.origin_start_stop_line_param.k));
		}
		else
		{
			y = Line_inter_point.y - FIXED_LENGHT * sin(atan(info.origin_start_stop_line_param.k));
			x = Line_inter_point.x - FIXED_LENGHT * cos(atan(info.origin_start_stop_line_param.k));
		}
		/*���ֱ�ߵĽǶ�*/
		target_line = line_fun(info.origin_Vehicle_XY.x,info.origin_Vehicle_XY.y,x,y);
		
		double degree = atan(target_line.k) * 180.0 / PI;
		
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
		
		/* */
		double gnss_Angle = strtod(gnss.CourseAngle,NULL);
		if(gnss_Angle > 180)
		{
			gnss_Angle = gnss_Angle - 360.0;
		}
		
		double err = gnss_Angle - target_degree;
		double fabs_err = fabs(err);
		
		tracking_control.value = fabs_err ;  //��һ��
		
		/*ȷ������*/
		if(err > 0 )                tracking_control.direct = VEHICLE_DIRECT_LEFT;
		else if(err < 0)            tracking_control.direct = VEHICLE_DIRECT_RIGHT;
		else                          tracking_control.direct = 0;
		
		
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
	NAV_output_t NAV_output;
	double Lenght,process = 0;
	double Angle,Speed = 60;
	pointToline_distance_t pointToline_info = {0};
	tracking_control_t     tracking_control = {0};
	if(waypoints_run_status.current_toindex != 0) //����ǰ����ʼ��
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
	if(Terminal_decision(GPStoXY(strtod(gnss.Lat,NULL),strtod(gnss.Lon,NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][0],NULL),strtod(wait_run_point[waypoints_run_status.current_toindex][1],NULL)).length))/*δ�����յ�*/
{
		/*�켣���ټ���*/
		tracking_control = tracking_control_Arith(&PID_angle_control,pointToline_info);
		
		
		
		/*������������߾���С��10cm=0.1m������Ϊ������*/
//		if(pointToline_info.pointToline > 0.5 || tracking_control.value > 90)//���ٹ���
//		{
//			/*��ʽת��*/
//			if(tracking_control.direct == VEHICLE_DIRECT_LEFT) Angle =  tracking_control.value * 1.3 ;    //����ֵ0~90
//			else                                               Angle = -tracking_control.value * 1.3;
//		}
//		else if(pointToline_info.pointToline < 0.5 && tracking_control.value < 30)
//		{
			/*��ʽת��*/
			if(tracking_control.direct == VEHICLE_DIRECT_LEFT)              Angle =  tracking_control.value ;    //����ֵ0~90
			else if(tracking_control.direct == VEHICLE_DIRECT_RIGHT)        Angle = -tracking_control.value ;
			else                                                            Angle = 0 ;
//		}
//		if(pointToline_info.pointToline < 0.03 && tracking_control.direct == 0) //��Ϊ�ں�������
//		{
//			Angle = 0;
//		}
		
		/*��������*/
		//SpeedĿǰ�ֶ���ֵ
		NAV_output.RSpeed =  Speed + Angle;
		NAV_output.LSpeed =  Speed - Angle;

		/*���ȼ���*/
		/*������������ľ������ڽ��ȼ���*/
		if((tracking_control.foot_point.x * pointToline_info.Endpoint_XY.x)>= 0 && (tracking_control.foot_point.y * pointToline_info.Endpoint_XY.y)>=0) //˵���������յ���һ������
		{
			/*���Ϊ��0��0�� ���Ⱦ�Ϊ������a����b��*/
			Lenght = sqrt(tracking_control.foot_point.x * tracking_control.foot_point.x + tracking_control.foot_point.y * tracking_control.foot_point.y );
		}
	  else
		{
			Lenght = 0;
		}
		process = Lenght/ (pointToline_info.StartToTerminal - TERMINAL_RANGE);
	
		if(process >= (TERMINAL_RANGE/ (pointToline_info.StartToTerminal - TERMINAL_RANGE))+1)//˵�������յ�
		{
			if(waypoints_run_status.current_toindex > 0)
			{
				waypoints_run_status.current_toindex++;
				waypoints_run_status.current_fromindex++;
			}
			else  waypoints_run_status.current_toindex++;
		}
			if(process > 1) //���ȸ�ֵΪ1
		{
			
		}
	}
	else /*�����յ�*/
	{
		if(waypoints_run_status.current_toindex > 0)
		{
			waypoints_run_status.current_toindex++;
			waypoints_run_status.current_fromindex++;
		}
		else  waypoints_run_status.current_toindex++;

		Vehicle_To_Distance_Angle_flag = 0;
	}
	printf("��������%f,����%d\r\n",Angle,tracking_control.direct);
	return NAV_output;
}



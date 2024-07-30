#include "change_status.h"
#include "nav.h"
#include "http.h"


Device_Poweron_status_t Device_Poweron_status = Check_poweron;

Change_Status_t Device_Run_Status =
{
	Poweron,
	Poweron,
	Job_Wait
};
extern uint32_t Device_unusual_time;
extern cJSON * EC600U_MQTT_SEND_STATUS;
extern osMessageQueueId_t HTTP_REQUEST_queueHandle;
extern osEventFlagsId_t Device_Run_status_eventHandle;
void change_status_fun()
{
	uint32_t BIT = 0;
	switch(Device_Run_Status.Curstatus)
	{
		case Job_Wait:  //��ǰ״̬Ϊ���У�ֻ����뵽��ҵ�� ֻ���http�л�ȡ���߲�����
//		/*�����ǵ�һ�����л��������й�������������,ֻҪ�ǿ�ʼ���񣬾���Ҫ���ñ�־λ*/
			NAV_Control_Param_clear();
		/*�Ƚ���http�õ��ĺ�����зָ�*/
			waypoints_Parse(HTTP_Task_Msg.waypoints,",");
			/*���õ�ǰ״̬*/
			Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
			Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
			/*����APP����״̬�仯��*/
			/*�����ٽ���*/
//					taskENTER_CRITICAL();
			
			Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
			/*�˳��ٽ���*/
//					taskEXIT_CRITICAL();
			
			/*���õ�23λ���豸��������*/
			Device_unusual_time   = 5000;
		break;
		
		case Job_Working: //���ڹ���״̬����ת����Ϊֹͣ�����У�����ҵ��ͣ����ҵ��ɣ��ٻ�,�����ϰ����
			switch(Device_Run_Status.Alterstatus)
			{
				case Job_Wait   : //�ӹ���״̬���������У�ֻ����ֹͣ��ť
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					NAV_Control_Param_clear(); 
				
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Pause  : //���ڹ���״̬ת������ͣ״̬ ��ͣ��ť
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
//						case Job_Finish : 
//							/*���õ�ǰ״̬*/
//							Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
//							Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
//							
//							/*�ϴ�APP״̬��Ϣ*/
//							/*�����ٽ���*/
//							taskENTER_CRITICAL();
//							Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
//							/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
//						
//							Device_Run_Status.Curstatus = Job_Wait;
//						break;
				case Job_Return :  //���ڹ���״̬ת��Ϊ�ٻ�״̬
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
					/*������ͣ������HTTP��¼��ǰ��ͣ��λ��*/
					BIT = BIT_2;
					osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
				
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					NAV_Control_Param_clear(); 
				
					/*����http�õ��ĺ�����зָ�*/
					waypoints_Parse(Charge_info.navWaypoints,",");
				
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Block  :   break; //��ʱ��д
				default:            break;
			}
			break;
		case Job_Pause:  //������ͣ״̬���Ը���Ϊ��ҵ�У��ٻ�����״̬   
			switch(Device_Run_Status.Alterstatus)
			{
				case Job_Wait   :   break;
				case Job_Working: //������ͣ״̬�任Ϊ��ҵ״̬
					if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_23) == 0) osEventFlagsSet(Device_Run_status_eventHandle,BIT_23);  //ת��Ϊ����
					
					/*����ڴ�working��pause����working���ùܴ�http contiune�����ȡ�ĺ��ߵ㣬����Ǵ�return��pause����continue����Ҫ�����ȡ�ĺ��ߣ��ж����ߵ�����Ϊ���Ƿ�ǰ���ߵ����3��,��Ϊÿ���ٻ��Ҷ������ߵ����������Ȼ��ѹ��ڳ��װλ�õ���������д�룬��ô��4��������Ϊ��*/
				if(wait_run_point[3][0] == '\0' || wait_run_point[4][0] == '\0')
					{
						NAV_Control_Param_clear();
						waypoints_Parse(HTTP_Task_Msg.waypoints,",");  /*������յ��ĺ���*/
					}
				
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
				
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Return :  
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					NAV_Control_Param_clear(); 
				
					/*����http�õ��ĺ�����зָ�*/
					waypoints_Parse(Charge_info.navWaypoints,",");
				
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
				default:            break;
			}
			break;
		case Job_Return     ://        
			switch(Device_Run_Status.Alterstatus)
			{
				case Job_Wait://�������
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
//							NAV_Control_Param_clear(); 
//						
//							/*����http�õ��ĺ�����зָ�*/
//							waypoints_Parse(Charge_info.navWaypoints,",");
				
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Pause://�˹������������
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //��������
					/*���õ�ǰ״̬*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
				
					/*�ϴ�APP״̬��Ϣ*/
					/*�����ٽ���*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*�˳��ٽ���*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Block  :   break;
				default:            break;
			}
			break;
		case Job_Block      :        break;  //̫���ӣ��Ȼ���˵
		default:                     break;
	}
}

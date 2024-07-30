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
		case Job_Wait:  //当前状态为空闲，只会进入到作业中 只会从http中获取航线并解析
//		/*无论是第一次运行还是在运行过程中重新运行,只要是开始任务，就需要重置标志位*/
			NAV_Control_Param_clear();
		/*先将从http拿到的航点进行分割*/
			waypoints_Parse(HTTP_Task_Msg.waypoints,",");
			/*设置当前状态*/
			Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
			Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
			/*告诉APP我们状态变化了*/
			/*进入临界区*/
//					taskENTER_CRITICAL();
			
			Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
			/*退出临界区*/
//					taskEXIT_CRITICAL();
			
			/*设置第23位让设备可以启动*/
			Device_unusual_time   = 5000;
		break;
		
		case Job_Working: //正在工作状态可以转化成为停止（空闲），作业暂停，作业完成，召回,遇到障碍物等
			switch(Device_Run_Status.Alterstatus)
			{
				case Job_Wait   : //从工作状态进入至空闲，只能是停止按钮
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					NAV_Control_Param_clear(); 
				
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Pause  : //正在工作状态转化成暂停状态 暂停按钮
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
//						case Job_Finish : 
//							/*设置当前状态*/
//							Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
//							Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
//							
//							/*上传APP状态信息*/
//							/*进入临界区*/
//							taskENTER_CRITICAL();
//							Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
//							/*退出临界区*/
//							taskEXIT_CRITICAL();
//						
//							Device_Run_Status.Curstatus = Job_Wait;
//						break;
				case Job_Return :  //正在工作状态转变为召回状态
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
					/*发布暂停请求，让HTTP记录当前暂停的位置*/
					BIT = BIT_2;
					osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
				
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					NAV_Control_Param_clear(); 
				
					/*将从http拿到的航点进行分割*/
					waypoints_Parse(Charge_info.navWaypoints,",");
				
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Block  :   break; //暂时不写
				default:            break;
			}
			break;
		case Job_Pause:  //正在暂停状态可以更换为作业中，召回两个状态   
			switch(Device_Run_Status.Alterstatus)
			{
				case Job_Wait   :   break;
				case Job_Working: //正在暂停状态变换为作业状态
					if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_23) == 0) osEventFlagsSet(Device_Run_status_eventHandle,BIT_23);  //转变为启动
					
					/*如果在从working至pause再至working则不用管从http contiune哪里获取的航线点，如果是从return至pause再至continue就需要处理获取的航线，判断两者的依据为看是否当前航线点大于3个,因为每次召回我都将航线点数组清除，然后把关于充电装位置的三个数据写入，那么第4个数据则为空*/
				if(wait_run_point[3][0] == '\0' || wait_run_point[4][0] == '\0')
					{
						NAV_Control_Param_clear();
						waypoints_Parse(HTTP_Task_Msg.waypoints,",");  /*处理接收到的航线*/
					}
				
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
				
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Return :  
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
					NAV_Control_Param_clear(); 
				
					/*将从http拿到的航点进行分割*/
					waypoints_Parse(Charge_info.navWaypoints,",");
				
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
				default:            break;
			}
			break;
		case Job_Return     ://        
			switch(Device_Run_Status.Alterstatus)
			{
				case Job_Wait://返航完成
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
					
//							NAV_Control_Param_clear(); 
//						
//							/*将从http拿到的航点进行分割*/
//							waypoints_Parse(Charge_info.navWaypoints,",");
				
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Pause://人工点击结束返航
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
					/*设置当前状态*/
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Device_Run_Status.Alterstatus;
				
					/*上传APP状态信息*/
					/*进入临界区*/
//							taskENTER_CRITICAL();
					Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Device_Run_Status.Curstatus,"task","tStatus");
					/*退出临界区*/
//							taskEXIT_CRITICAL();
				break;
				case Job_Block  :   break;
				default:            break;
			}
			break;
		case Job_Block      :        break;  //太复杂，等会再说
		default:                     break;
	}
}

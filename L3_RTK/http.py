import request
import ujson

def http_request(fun,url,data,Header):
    try:
        if(fun == 'post'):
            response = request.post(url,data = data, headers = Header)
        elif(fun == 'get'):
            response = request.get(url,data = data, headers = Header)
        from usr.test.usart import stm32_usart
        stm32_usart.Queue_put("{" + "\"Name\":\"HTTP\",\"url\":\"{}\",\"msg\":{}".format(url,response.json()) + "}")
    except Exception as e:
        print("HTTP获取失败\r\n")
        stm32_usart.Queue_put("{" + "\"Name\":\"HTTP\",\"url\":\"{}\",\"msg\": \"fail\"".format(url) + "}")

# HTTP_data_semp = _thread.allocate_semphore(1)

# HTTP_CMD = 0

# #获取认证函数
# def http_device_Authentication(checkSum):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/authentication',data = "checkSum={}".format(checkSum), headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8'})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         data = data.get('data')
#         config.device_info['companyId'] = data.get('companyId')
#         config.device_info['deviceId']  = data.get('deviceId')
#         config.device_info['groupId']   = data.get('groupId')
#         #修改APP状态上报信息
#         config.App_MQTT_device_status_info['property']['cId'] = config.device_info['companyId']
#         config.App_MQTT_device_status_info['property']['dId'] = config.device_info['deviceId']
#         config.App_MQTT_device_status_info['property']['gId'] = config.device_info['groupId']

#         #获取APP发布订阅主题
#         config.App_MQTT['Sub_Topic']    = "xzb/autopilot/mower/"+str(config.device_info['companyId'])+"/device/service/call/"+str(config.device_info['groupId'])+'/'+str(config.device_info['deviceId'])
#         config.App_MQTT['Pub_Topic']    = "xzb/autopilot/mower/"+str(config.device_info['companyId'])+"/device/property/post/"+str(config.device_info['groupId'])+'/'+str(config.device_info['deviceId'])
#         print(config.App_MQTT['Pub_Topic'])
#         print(config.App_MQTT['Sub_Topic'])

#         usart.stm32_usart.Queue_put("type:Authen_success")
#         #um482上电
#         usart.stm32_usart_send_apply = _thread.start_new_thread(usart.stm32_usart_send_task, ())   # 创建一个线程，当函数无参时传入空的元组
#         usart.stm32_usart_rec__apply     = _thread.start_new_thread(usart.stm32_usart_rec_task, ())   # 创建一个线程，当函数无参时传入空的元组
#     else:
#         print("产品未注册")
#         usart.stm32_usart.Queue_put("Authen_fail")

# #获取开始作业
# def http_device_task_Start(zoneId,deviceId,groupId):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/jobStart',data = "zoneId={}".format(zoneId), headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8','deviceId':'{}'.format(deviceId),'groupId':'{}'.format(groupId)})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         print('开始作业')
#         usart.stm32_usart.Queue_put('type:{};points:{};taskNum:{}'.format('start',data.get('waypoints'),data.get('taskNum')))
#     else:
#         print("开始作业获取失败")

# #获取继续作业
# def http_device_task_Continue(zoneId,deviceId,groupId):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/jobContinue',data = "zoneId={}".format(zoneId), headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8','deviceId':'{}'.format(deviceId),'groupId':'{}'.format(groupId)})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         print('继续作业')
#         usart.stm32_usart.Queue_put('type:{};points:{};targetIndex:{};taskNum:{}'.format('contiune',data.get('waypoints'),data.get('targetIndex'),data.get('taskNum')))
#     else:
#         print("继续作业获取失败")

# #获取暂停作业 
# # reqest_param ={
# #   "pauseLat": 0,
# #   "pauseLon": 0,
# #   "progress": 0,
# #   "targetIndex": 0,
# #   "zoneId": 0
# # }
# def http_device_task_Pause(reqest_param,deviceId,groupId):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/jobPause', data = "{}".format(ujson.dumps(reqest_param)),headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8','deviceId':'{}'.format(deviceId),'groupId':'{}'.format(groupId)})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         print('暂停作业')
#     else:
#         print("暂停作业获取失败")

# #任务完成
# def http_device_task_Finish(pointNum,zoneId,deviceId,groupId):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/jobFinish',data = "zoneId={}&pointNum={}".format(zoneId,pointNum), headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8','deviceId':'{}'.format(deviceId),'groupId':'{}'.format(groupId)})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         print('完成作业')

#     else:
#         print("完成作业获取失败")

# #获取分段航点
# # reqest_param ={
# #   "progress": 0,
# #   "size": 0,
# #   "startIndex": 0,
# #   "tarIndex": 0,
# #   "zoneId": 0
# # }
# def http_device_updateRoute(reqest_param,deviceId,groupId):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/updateRoute',data = "{}".format(ujson.dumps(reqest_param)), headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8','deviceId':'{}'.format(deviceId),'groupId':'{}'.format(groupId)})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         print('获取分段航点成功')
#         usart.stm32_usart.Queue_put('type:{};points:{};targetIndex:{};taskNum:{}'.format('updateRoute',data.get('waypoints'),data.get('targetIndex'),data.get('taskNum')))
#     else:
#         print("完成作业获取失败")

# #获取充电桩位置 用于一键召回
# def http_device_gotoCharge(zoneId,deviceId,groupId):
#     response = request.post('http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/goToCharge',data = "zoneId={}".format(ujson.dumps(zoneId)), headers = {'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8','deviceId':'{}'.format(deviceId),'groupId':'{}'.format(groupId)})
#     data = response.json()
#     print(data)
#     if (data.get('code') == 100):
#         print('获取充电桩位置')
#         usart.stm32_usart.Queue_put('type:{};points:{};'.format('charge',data.get('navWaypoints')))
#     else:
#         print("完成作业获取失败")

# def HTTP_data_task():
#     while True:
#         err = HTTP_data_semp.acquire()
#         if err :
#             #HTTP获取认证 成功返回companyId deviceId groupId 
#             if(HTTP_CMD == 0):  # 开机执行  从http
#                 http_device_Authentication(config.Device_IMEI)
#             elif(HTTP_CMD == 1): #其他命令
#                 print("其他命令")

#         else :
#             print("HTTP传来数据失败\r\n")
#         utime.sleep(1)


import _thread
import utime
import log
import net
import ujson
import dataCall

from queue import Queue

from umqtt import MQTTClient
from usr.test import config

MQTT_APP_data_apply = ''

# 设置日志输出级别
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")

MQTT_Base_data_semp = _thread.allocate_semphore(1)
MQTT_Base_data_semp.acquire()

MQTT_APP_data_semp = _thread.allocate_semphore(1)
MQTT_APP_data_semp.acquire()

MQTT_self_Base_data_semp = _thread.allocate_semphore(1)
MQTT_self_Base_data_semp.acquire()

MQTT_APP_queue = Queue()

#通过MQTT发送车辆状态
def MQTT_APP_data_pub():
    if(MQTT_APP.conn_status == 1):
        while True :
            data = MQTT_APP_queue.get()
            if data != None :
                try:
                    MQTT_APP.publish(config.App_MQTT['Pub_Topic'],ujson.dumps(data))
                except Exception as e:
                    print("MQTT发布失败")
                    from usr.test.usart import stm32_usart
                    stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"APP_pub\",\"msg\":\"fail\"" + "}")
            else :
                print(" MQTT_APP发布信息为空\r\n")

# 接收到APP数据后处理函数
def MQTT_APP_data_sub_cb(topic, msg):
    from usr.test.usart import stm32_usart
    stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"APP\",\"msg\":{}".format(msg.decode()) + "}")
    # data = ujson.loads(msg.decode())
    # print(data,type(data))
    # if(data['type'] == 2001):
    #     print('启动任务')
    #     http.http_device_task_Start(data['data']['zoneId'],config.device_info['deviceId'],config.device_info['groupId']) #zoneId,deviceId,groupId
    # elif(data['type'] == 2002):
    #     print('暂停任务')
    #     from usr.test.usart import stm32_usart
    #     stm32_usart.Queue_put('type:Pause')
    # elif(data['type'] == 2003):
    #     print('继续任务')
    #     http.http_device_task_Continue(data['data']['zoneId'],config.device_info['deviceId'],config.device_info['groupId']) #zoneId,deviceId,groupId
    # elif(data['type'] == 2004):
    #     print('一键召回')
    #     http.http_device_gotoCharge()
    # elif(data['type'] == 2005):
    #     print('割刀高度')
    # elif(data['type'] == 2006):
    #     print('安全区域检测')
    # elif(data['type'] == 2007):
    #     print('修改安全区域检测')
    # elif(data['type'] == 2008):
    #     print('车灯控制')
    # elif(data['type'] == 2009):
    #     print('割刀控制')

# MQTT连接app代码处理

def MQTT_APP_data_task():
    global MQTT_APP
    global MQTT_APP_data_apply
    while True:
        err = MQTT_APP_data_semp.acquire()
        if err :
            if(MQTT_APP.conn_status == 1):
                    print("MQTT_APP已连接成功")
            else:
                print("开始连接app")
                MQTT_APP.topic = config.App_MQTT['Sub_Topic']
                MQTT_APP.set_callback(MQTT_APP_data_sub_cb)
                try:
                    MQTT_APP.connect() 
                    print("连接APP成功")
                    MQTT_APP.subscribe(MQTT_APP.topic)
                    from usr.test.usart import stm32_usart
                    stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"{}\",\"msg\":\"success\"".format(config.App_MQTT['client_id']) + "}")
                    MQTT_APP.conn_status = 1
                    MQTT_APP_data_apply = _thread.start_new_thread(MQTT_APP_data_pub, ())   # 创建一个线程，当函数无参时传入空的元组
                    # 监听mqtt消息
                    MQTT_APP.loop_forever()
                except Exception as e:
                    print("打开APP_MQTT失败")
                    if(MQTT_APP.reconnect() == True):
                        print("连接APP成功")
                        MQTT_APP.conn_status = 1
                        # 监听mqtt消息
                        MQTT_APP.loop_forever()
                    else:
                        print("APP重连失败。请重新发出指令")
                        config.reconn_fail_apply()
        else :
            print("MQTT传来数据失败\r\n")
        utime.sleep_ms(10)

# 接收到基站数据后处理函数
def MQTT_Base_data_sub_cb(topic, msg):
    # mqtt_log.info("Subscribe Recv: Topic={},Msg={}".format(topic.decode(), msg))
    if(MQTT_Base.conn_status == 1):
            if(len(msg) > 500):
                # print(len(msg))
                from usr.test.um482 import um482
                um482.Queue_put(msg)

# MQTT接受已运行基站的数据处理代码
def MQTT_Base_data_task():
    global MQTT_Base
    while True:
        err = MQTT_Base_data_semp.acquire()
        if err == True:
            if(MQTT_Base.conn_status == 1): # 没用到
                # print("MQTT")
                from usr.test.um482 import um482
                um482.Queue_put(MQTT_Base.msg_data)
            else:
                MQTT_Base.topic = config.Base_MQTT['Topic']
                # 设置消息回调
                MQTT_Base.set_callback(MQTT_Base_data_sub_cb)
                try:
                    MQTT_Base.connect()
                    config.device_Mode['Current_Source'] = config.device_Mode['Alter_Source']
                    config.device_Mode['Current_Mode'] = 'Moving'
                    config.device_Mode['Alter_Source'] = "1"
                    config.device_Mode['Alter_Mode']   = "1"
                    MQTT_Base.conn_status = 1
                    MQTT_Base.subscribe(MQTT_Base.topic)
                    print("基站订阅成功")
                    from usr.test.usart import stm32_usart
                    stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"{}\",\"msg\":\"success\"".format(config.Base_MQTT['client_id']) + "}")
                    if( config.ble_Isopen == 1 ):
                        #蓝牙消息回复
                        config.R_WorkMode(config.device_Mode['Current_Mode'])
                    # 监听mqtt消息
                    MQTT_Base.loop_forever()
                except Exception as e:
                    print("打开基站MQTT失败")
                    if(MQTT_Base.reconnect() == True):
                        config.device_Mode['Current_Source'] = config.device_Mode['Alter_Source']
                        config.device_Mode['Current_Mode'] = 'Moving'
                        config.device_Mode['Alter_Source'] = "1"
                        config.device_Mode['Alter_Mode']   = "1"
                        MQTT_Base.conn_status = 1
                        print("基站订阅成功")
                        if( config.ble_Isopen == 1 ):
                            #蓝牙消息回复
                            config.R_WorkMode(config.device_Mode['Current_Mode'])
                        # 监听mqtt消息
                        MQTT_Base.loop_forever()
                    else:
                        print("基站重连失败。请重新发出指令")
                        config.reconn_fail_apply()
        else :
            print("MQTT传来数据失败\r\n")
        
# 自建为基站代码处理
def MQTT_self_Base_data_task():
    global MQTT_self_Base
    while True:
        err = MQTT_self_Base_data_semp.acquire()
        if err :
            try:
                MQTT_self_Base.connect()
                print("自建基站MQTT连接成功")
                MQTT_self_Base.conn_status = 1
                config.device_Mode['Current_Source'] = ''
                config.device_Mode['Current_Mode'] = 'Base'
                config.device_Mode['Alter_Source'] = ''
                config.device_Mode['Alter_Mode']   = ''
                from usr.test.usart import stm32_usart
                stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"{}\",\"msg\":\"success\"".format(config.Base_self_MQTT['client_id']) + "}")
                if( config.ble_Isopen == 1 ):
                    #蓝牙消息回复
                    config.R_WorkMode(config.device_Mode['Current_Mode'])
                # 监听mqtt消息
                MQTT_self_Base.loop_forever()
            except Exception as e:
                print("打开自建基站MQTT失败")
                if(MQTT_self_Base.reconnect() == True ):
                    print("自建基站MQTT连接成功")
                    MQTT_self_Base.conn_status = 1
                    config.device_Mode['Current_Source'] = ''
                    config.device_Mode['Current_Mode'] = 'Base'
                    config.device_Mode['Alter_Source'] = ''
                    config.device_Mode['Alter_Mode']   = ''
                    if( config.ble_Isopen == 1 ):
                        #蓝牙消息回复
                        config.R_WorkMode(config.device_Mode['Current_Mode'])
                    # 监听mqtt消息
                    MQTT_self_Base.loop_forever()
                else:
                    print("自建重连失败。请重新发出指令")
                    config.reconn_fail_apply()
        utime.sleep_ms(10)

class mqtt(object):
    '''
    mqtt init
    '''

    # 说明：reconn该参数用于控制使用或关闭umqtt内部的重连机制，默认为True，使用内部重连机制。
    # 如需测试或使用外部重连机制可参考此示例代码，测试前需将reconn=False,否则默认会使用内部重连机制！
    def __init__(self, clientid, server, port, user, password,semphore ,send_fun_handle = None,send_fun = None, keepalive=0, ssl=False, ssl_params={},
                 reconn=False):
        self.conn_status = 0
        self.__clientid = clientid
        self.__pw = password
        self.__server = server
        self.__port = port
        self.__uasename = user
        self.__keepalive = keepalive
        self.__ssl = ssl
        self.__ssl_params = ssl_params
        self.semphore = semphore
        self.__send_fun_hand= send_fun_handle
        self.__send_fun= send_fun
        self.topic = None
        self.qos = None
        self.recon_num = 0
        self.TaskEnable = True
        # 网络状态标志
        self.__nw_flag = True
        # 创建互斥锁
        self.mp_lock = _thread.allocate_lock()
        # 创建类的时候初始化出mqtt对象
        self.client = MQTTClient(self.__clientid, self.__server, self.__port, self.__uasename, self.__pw,
                                 keepalive=self.__keepalive, ssl=self.__ssl, ssl_params=self.__ssl_params,
                                 reconn=False)

        # 设置异常回调
        self.error_register_cb(self.err_cb)

    def connect(self):
        '''
        连接mqtt Server
        '''
        return self.client.connect()
         

    
    def err_cb(self,error):
        '''
        接收umqtt线程内异常的回调函数
        '''
        mqtt_log.info(error)
        self.reconnect() # 可根据异常进行重连

    def set_callback(self, sub_cb):
        '''
        设置mqtt回调消息函数
        '''
        self.client.set_callback(sub_cb)

    def error_register_cb(self, func):
        '''
        注册一个接收umqtt内线程异常的回调函数
        '''
        self.client.error_register_cb(func)

    def subscribe(self, topic, qos=0):
        '''
        订阅Topic
        '''
        self.topic = topic  # 保存topic ，多个topic可使用list保存
        self.qos = qos  # 保存qos
        self.client.subscribe(topic, qos)

    def publish(self, topic, msg, qos=0):
        '''
        发布消息
        '''
        self.client.publish(topic, msg, qos)

    def disconnect(self):
        '''
        关闭连接
        '''
        if(self.conn_status == 1):
            self.conn_status = 0
            # 关闭wait_msg的监听线程
            self.TaskEnable = False
            # 关闭之前的连接，释放资源
            self.client.disconnect()
            if self.__send_fun != None :
                _thread.stop_thread(self.__send_fun)
            from usr.test.usart import stm32_usart
            stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"{}\",\"msg\":\"fail\"".format(self.__clientid) + "}")

    def reconnect(self):
        '''
        mqtt 重连机制(该示例仅提供mqtt重连参考，根据实际情况调整)
        PS：1.如有其他业务需要在mqtt重连后重新开启，请先考虑是否需要释放之前业务上的资源再进行业务重启
            2.该部分需要自己根据实际业务逻辑添加，此示例只包含mqtt重连后重新订阅Topic
        '''
        
        # 判断锁是否已经被获取
        if self.mp_lock.locked():
            return
        self.mp_lock.acquire()
        if self.conn_status == 1 :
            # 重新连接前关闭之前的连接，释放资源(注意区别disconnect方法，close只释放socket资源，disconnect包含mqtt线程等资源)
            self.client.close()
        # 重新建立mqtt连接
        while True:
            if(self.recon_num <= 5): 
                net_sta = net.getState()  # 获取网络注册信息
                # 注册完成
                if net_sta != -1 and net_sta[1][0] == 1:
                    call_state = dataCall.getInfo(1, 0)  # 获取拨号信息
                    # 拨号成功
                    if (call_state != -1) and (call_state[2][0] == 1):
                        try:
                            # 网络正常，重新连接mqtt
                            self.connect()
                            if(self.topic != None):
                                self.subscribe(self.topic)
                            self.conn_status = 1
                            if (self.__send_fun != None)  and (self.__send_fun_hand !=None) :
                                self.__send_fun_hand = _thread.start_new_thread(self.__send_fun, ())   # 创建一个线程，当函数无参时传入空的元组
                            self.recon_num = 0
                        except Exception as e:
                            self.recon_num = self.recon_num + 1
                            # 重连mqtt失败, 1s继续尝试下一次
                            self.client.close()
                            utime.sleep(1)
                            continue
                    else:
                        self.recon_num = self.recon_num + 1
                        # 网络未恢复，等待恢复
                        utime.sleep(5)
                        continue
                else:
                    self.recon_num = self.recon_num + 1
                    utime.sleep(5)
                    continue
                # 退出重连
                print("重连成功\r\n")
                from usr.test.usart import stm32_usart
                stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"{}\",\"msg\":\"recon_success\"".format(self.__clientid) + "}")
                self.recon_num = 0
                return True
            else:
                print("重连失败\r\n")
                self.recon_num = 0
                from usr.test.usart import stm32_usart
                stm32_usart.Queue_put("{" + "\"Name\":\"MQTT\",\"fun\":\"{}\",\"msg\":\"recon_fail\"".format(self.__clientid) + "}")
                return False

    def __listen(self):
        while True:
            try:
                if not self.TaskEnable:
                    break
                self.client.wait_msg()
            except OSError as e:
                # 判断网络是否断线
                if not self.__nw_flag:
                    self.conn_status = 0
                    # 网络断线等待恢复进行重连
                    self.reconnect()
                # 在socket状态异常情况下进行重连
                elif self.client.get_mqttsta() != 0 and self.TaskEnable:
                    self.reconnect()
                else:
                    # 这里可选择使用raise主动抛出异常或者返回-1
                    return -1

    def loop_forever(self):
        _thread.start_new_thread(self.__listen, ())


MQTT_APP       = mqtt(config.App_MQTT['client_id'],config.App_MQTT['IP'],config.App_MQTT['port'],config.App_MQTT['username'],config.App_MQTT['password'],semphore = MQTT_APP_data_semp,send_fun_handle = MQTT_APP_data_apply,send_fun = MQTT_APP_data_pub)
MQTT_Base      = mqtt(config.Base_MQTT['client_id'],config.Base_MQTT['IP'],config.Base_MQTT['port'],config.Base_MQTT['username'],config.Base_MQTT['password'],semphore = MQTT_Base_data_semp)
MQTT_self_Base = mqtt(config.Base_self_MQTT['client_id'],config.Base_self_MQTT['IP'],config.Base_self_MQTT['port'],config.Base_self_MQTT['username'],config.Base_self_MQTT['password'],semphore = MQTT_self_Base_data_semp)

# MQTT_APP_time = timer(1000)
from machine import UART
import utime
import _thread
import ujson
from queue import Queue

from usr.test import config
from usr.test import socket
from usr.test import http
from usr.test import mqtt
from usr.test import um482
from usr.test import ble_drive

stm32_usart_data = []

stm32_usart_semp = _thread.allocate_semphore(15)

for index in range(15):
    stm32_usart_semp.acquire()

stm32_usart_status = 1

stm32_usart_send_apply= ''
stm32_usart_rec__apply= ''
um482_data_apply= ''
SOCKET_data_apply = ''
SOCKET_reset_apply = ''
MQTT_Base_data_apply= ''
MQTT_APP_data_apply= ''
MQTT_self_Base_data_apply= ''

def stm32_usart_send_task():
    if(stm32_usart_status == 1):
        while True:
            data = stm32_usart.queue.get()
            if data != None :
                    stm32_usart.uartWrite(data)
            else :
                print("um482传来数据失败\r\n")
            utime.sleep_ms(um482.RTK_HZ)

def stm32_usart_rec_task():
    global stm32_usart
    global um482_data_apply
    global SOCKET_data_apply
    global SOCKET_reset_apply
    global MQTT_Base_data_apply
    global MQTT_APP_data_apply
    global MQTT_self_Base_data_apply
    while True:
        err = stm32_usart_semp.acquire()
        if err :
            try:
                usart_data = ujson.loads(stm32_usart_data[0])
                #开机指令
                if(usart_data['Name'] =="OPEN"):
                    print("开机")
                    um482_data_apply = _thread.start_new_thread(um482.um482_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                    SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                    SOCKET_reset_apply   = _thread.start_new_thread(socket.SOCKET_reset_task, ())   # 创建一个线程，当函数无参时传入空的元组
                    MQTT_Base_data_apply = _thread.start_new_thread(mqtt.MQTT_Base_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                    MQTT_self_Base_data_apply  = _thread.start_new_thread(mqtt.MQTT_self_Base_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                    config.device_Mode['Alter_Source'] = usart_data['Source']
                    config.device_Mode['Alter_Mode']   = usart_data['fun']
                    ble_drive.ble_open_init()
                    if(config.device_Mode['Alter_Mode']    == 'Base') :
                        um482.um482.set_mode(0)
                        config.device_Mode['Current_Mode'] = 'Base'
                        config.device_Mode['Current_Source']  = ''
                        mqtt.MQTT_self_Base = mqtt.mqtt(config.Base_self_MQTT['client_id'],config.Base_self_MQTT['IP'],config.Base_self_MQTT['port'],config.Base_self_MQTT['username'],config.Base_self_MQTT['password'],mqtt.MQTT_self_Base_data_semp)
                        mqtt.MQTT_self_Base_data_semp.release()
                    else:
                        config.device_Mode['Current_Mode']    = 'Moving'
                        config.device_Mode['Current_Source']  = config.device_Mode['Alter_Source']
                        um482.um482.set_mode(1)
                        if(config.device_Mode['Alter_Source']  == 'dif_base'):
                            mqtt.MQTT_Base.conn_status = 0
                            mqtt.MQTT_Base = mqtt.mqtt(config.Base_MQTT['client_id'],config.Base_MQTT['IP'],config.Base_MQTT['port'],config.Base_MQTT['username'],config.Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
                            mqtt.MQTT_Base_data_semp.release()
                        else:
                            socket.socket_long_con = 0
                            socket.sock = socket.socket()
                            if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                                socket.SOCKET_data_semp.release()
                #切换模式指令
                elif(usart_data['Name'] =="Mode"):
                    if(usart_data['fun'] == "change_Mode"):
                        print('转换模式')
                        config.device_Mode['Last_Mode'] = config.device_Mode['Current_Mode']
                        if(config.device_Mode['Current_Mode'] == 'Moving'):
                            print('转变为基站模式')
                            um482.um482.set_mode(0)
                            if(config.device_Mode['Current_Source'] == 'dif_base'):
                                mqtt.MQTT_Base.disconnect()
                            else:
                                config.device_Mode['Last_Source'] = config.device_Mode['Current_Source']
                                _thread.stop_thread(SOCKET_data_apply)
                                socket.socket_long_con = 0
                                socket.socket_time.stop()
                                socket.sock.close()
                            mqtt.MQTT_self_Base = mqtt.mqtt(config.Base_self_MQTT['client_id'],config.Base_self_MQTT['IP'],config.Base_self_MQTT['port'],config.Base_self_MQTT['username'],config.Base_self_MQTT['password'],mqtt.MQTT_self_Base_data_semp)
                            mqtt.MQTT_self_Base_data_semp.release()
                        elif(config.device_Mode['Current_Mode'] == 'Base'):
                            print('转变为游端模式')
                            config.device_Mode['Alter_Source'] = usart_data['Source']
                            config.device_Mode['Current_Mode'] = 'Moving'
                            config.device_Mode['Alter_Mode']   = ''
                            mqtt.MQTT_self_Base.disconnect()
                            um482.um482.set_mode(1)
                            if(config.device_Mode['Alter_Source'] == 'dif_base'):
                                mqtt.MQTT_Base.conn_status = 0
                                mqtt.MQTT_Base = mqtt.mqtt(config.Base_MQTT['client_id'],config.Base_MQTT['IP'],config.Base_MQTT['port'],config.Base_MQTT['username'],config.Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
                                mqtt.MQTT_Base_data_semp.release()
                                config.device_Mode['Current_Source'] = 'dif_base'
                            else:
                                socket.socket_long_con = 0
                                SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                                socket.sock = socket.socket()
                                if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                                    socket.SOCKET_data_semp.release()

                    #切换源指令
                    elif(usart_data['fun'] == "change_source"):
                        print('转换源')
                        if(config.device_Mode['Current_Mode'] == 'Moving' and usart_data['Source'] != config.device_Mode['Current_Source']):
                            
                            config.device_Mode['Alter_Source'] = usart_data['Source']
                            print(config.device_Mode['Current_Source'])
                            print('转换为：{}'.format(config.device_Mode['Alter_Source']))
                            if(config.device_Mode['Current_Source'] == 'dif_base'):
                                mqtt.MQTT_Base.disconnect()
                                socket.socket_long_con = 0
                                SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                                socket.sock = socket.socket()
                                if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                                    socket.SOCKET_data_semp.release()
                            else:
                                config.device_Mode['Last_Source'] = config.device_Mode['Current_Source']
                                _thread.stop_thread(SOCKET_data_apply)
                                socket.socket_time.stop()
                                socket.sock.close()
                                socket.socket_long_con = 0
                                if(config.device_Mode['Alter_Source'] == 'dif_base'):
                                    mqtt.MQTT_Base = mqtt.mqtt(config.Base_MQTT['client_id'],config.Base_MQTT['IP'],config.Base_MQTT['port'],config.Base_MQTT['username'],config.Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
                                    mqtt.MQTT_Base_data_semp.release()
                                else:
                                    socket.socket_long_con = 0
                                    SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                                    socket.sock = socket.socket()
                                    if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                                        socket.SOCKET_data_semp.release()
                #操作socket 暂时未使用
                elif(usart_data['Name'] == 'socket'):
                    print("配置SOCKET")
                
                #操作Mqtt
                elif(usart_data['Name'] == "MQTT"):
                    if(usart_data['fun'] == "Base"):
                        print('操作基站Mqtt')
                    elif(usart_data['fun'] == "APP_open"):
                        config.App_MQTT['IP']        =  usart_data['Source']['IP']
                        config.App_MQTT['port']      =  usart_data['Source']['port']
                        config.App_MQTT['username']  =  usart_data['Source']['username']
                        config.App_MQTT['password']  =  usart_data['Source']['password']
                        config.App_MQTT['Sub_Topic']  =  usart_data['Source']['sub']
                        mqtt.MQTT_APP = mqtt.mqtt(config.App_MQTT['client_id'],config.App_MQTT['IP'],config.App_MQTT['port'],config.App_MQTT['username'],config.App_MQTT['password'],mqtt.MQTT_APP_data_semp)
                        mqtt.MQTT_APP_data_semp.release()
                    elif(usart_data['fun'] == "APP_pub"):
                        if(mqtt.MQTT_APP.conn_status == 1):
                            if(config.App_MQTT['Pub_Topic'] != usart_data['Source']['pub']):
                                config.App_MQTT['Pub_Topic'] = usart_data['Source']['pub']
                            # config.App_MQTT_device_status_info = usart_data['Source']['msg']
                            # mqtt.MQTT_APP.publish(config.App_MQTT['Pub_Topic'],ujson.dumps(config.App_MQTT_device_status_info))
                            mqtt.MQTT_APP_queue.put(usart_data['Source']['msg'])
                #操作http
                elif(usart_data['Name'] == "HTTP"):
                    print(usart_data)
                    http.http_request(usart_data['fun'],usart_data['Source']['url'],usart_data['Source']['data'],usart_data['Source']['Header'])
                
                elif(usart_data['Name'] == "GPS"):
                    print("GPS")


                elif(usart_data.find("BT") != -1):
                    print("配置BT")
                else:
                    print("发送不正确")
            except Exception as e:
                print("转化json出错\r\n")
                print(stm32_usart_data[0])
            del stm32_usart_data[0]
        else :
            print("stm32传来命令获取失败\r\n")
        utime.sleep_ms(10)

'''
 * 参数1：端口
        注：EC100YCN平台与EC600SCN平台，UARTn作用如下
        UART0 - DEBUG PORT
        UART1 – BT PORT
        UART2 – MAIN PORT
        UART3 – USB CDC PORT
 * 参数2：波特率
 * 参数3：data bits  （5~8）
 * 参数4：Parity  （0：NONE  1：EVEN  2：ODD）
 * 参数5：stop bits （1~2）
 * 参数6：flow control （0: FC_NONE  1：FC_HW）
'''

def reset_uart():
    global stm32_usart
    global stm32_usart_status
    global stm32_usart_send_apply
    stm32_usart.close()
    utime.sleep(1)
    stm32_usart = usart(UART.UART2,int(config.UART_para['bate']),int(config.UART_para['data_bits']),int(config.UART_para['parity']),int(config.UART_para['stop_bits']))
    stm32_usart_status = 1
    stm32_usart_send_apply = _thread.start_new_thread(stm32_usart_send_task, ())   # 创建一个线程，当函数无参时传入空的元组

class usart(object):
    def __init__(self,id, bate=460800, data_bits=8, parity=0, stop_bits=1,flow_control=0,semphore = None):
        self.semp = semphore
        self.queue = Queue()
        self.uart = UART(id, bate, data_bits, parity, stop_bits, flow_control)
        self.uart.set_callback(self.callback)
        

    def callback(self, para):
        if(0 == para[0]):
            stm32_usart_data.append(self.uartRead(para[2]))
            if(self.semp.getCnt().curCnt < 15):
                self.semp.release()

    def Queue_put(self , data):
        return self.queue.put(data)

    def uartWrite(self, msg):
        self.uart.write(msg)

    def uartRead(self, len):
        return self.uart.read(len).decode()
    
    def close(self):
        self.uart.close()

stm32_usart     = usart(UART.UART2,semphore = stm32_usart_semp)
    
    

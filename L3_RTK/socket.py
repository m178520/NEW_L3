import _thread
import log
import utime
import checkNet

import usocket
import ubinascii

from usr.test import config
from usr.test.um482 import um482
from usr.test.timer import timer

'''
下面两个全局变量是必须有的，用户可以根据自己的实际项目修改下面两个全局变量的值
'''
PROJECT_NAME = "QuecPython_Socket_example"
PROJECT_VERSION = "1.0.0"


checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)
reset_num = 0
Isreset = 0


# 设置日志输出级别
log.basicConfig(level=log.INFO)
socket_log = log.getLogger("SOCKET")

SOCKET_data_semp = _thread.allocate_semphore(1)
SOCKET_data_semp.acquire()

SOCKET_resert_semp = _thread.allocate_semphore(1)
SOCKET_resert_semp.acquire()

socket_long_con = 0

def time_cb(arg):
    global sock
    global socket_long_con
    if(um482.GPS_socket != ''):
        try:
            sock.send_data(bytes('{}\r\n'.format(um482.GPS_socket),'utf-8'))
            if(socket_long_con == 0):
                socket_long_con = 1
                if(SOCKET_data_semp.getCnt().curCnt != 1):
                    SOCKET_data_semp.release()
        except Exception as e:
            socket_time.stop()
            print("定时器")

def SOCKET_reset_task():
    while True:
        err = SOCKET_resert_semp.acquire()
        if err == True :
            config.socket_reset()

def SOCKET_data_task():
    global sock
    global reset_num
    global socket_long_con
    while True:
        if(socket_long_con == 1):
            try:
                socket_data = sock.rec_data()
                if(len(socket_data)>150):
                    um482.Queue_put(socket_data)
                utime.sleep(1)
            except Exception as e:
                print("获取错误")
                if(SOCKET_resert_semp.getCnt().curCnt != 1):
                        SOCKET_resert_semp.release()
                    
        else:
            err = SOCKET_data_semp.acquire()
            if err == True :
                if(socket_long_con != 1):
                    print("配置SOCKET\r\n")
                    # 创建一个socket实例
                    try: # 获取挂载点
                        # 建立连接
                        if(config.device_Mode['Alter_Source'] == 'qxwz'):       sock.connect(config.qxwz['url'],config.qxwz['port'])
                        elif(config.device_Mode['Alter_Source'] == 'starcart'): sock.connect(config.starcart['url'],config.starcart['port'])
                        elif(config.device_Mode['Alter_Source'] == 'huacenav'): sock.connect(config.huacenav['url'],config.huacenav['port'])
                        elif(config.device_Mode['Alter_Source'] == 'pnt'):      sock.connect(config.pnt['url'],config.pnt['port'])
                        elif(config.device_Mode['Alter_Source'] == 'sixents'):  sock.connect(config.sixents['url'],config.sixents['port'])
                        elif(config.device_Mode['Alter_Source'] == 'tencent'):  sock.connect(config.tencent['url'],config.tencent['port'])
                        # 向服务端发送消息
                        sock.send_data("GET / HTTP/1.0\r\nUser-Agent:NTRIP AIR724_RTK/1.0.0\r\nAccept: */*\r\nConnection: close\r\n\r\n")
                        #接收服务端消息
                        data=sock.rec_data()
                        sock.__nw_flag = True
                        print("{}".format(data))
                        if(data.find("200 OK") != -1):
                            first_addr = 200
                            while(data.find("STR",first_addr) != -1):
                                first_addr=data.find("STR",first_addr)+4
                                str = data[first_addr:first_addr+20].split(';',1)
                                config.qxwz['MOintPoint'].append(str[0])
                            sock.close()
                            if(len(config.qxwz['MOintPoint']) != 0):         ## 进入长链接
                                # 创建一个socket实例
                                sock = socket()
                                try:
                                    # 建立连接
                                    if(config.device_Mode['Alter_Source'] == 'qxwz'):       sock.connect(config.qxwz['url'],config.qxwz['port'])
                                    elif(config.device_Mode['Alter_Source'] == 'starcart'): sock.connect(config.starcart['url'],config.starcart['port'])
                                    elif(config.device_Mode['Alter_Source'] == 'huacenav'): sock.connect(config.huacenav['url'],config.huacenav['port'])
                                    elif(config.device_Mode['Alter_Source'] == 'pnt'):      sock.connect(config.pnt['url'],config.pnt['port'])
                                    elif(config.device_Mode['Alter_Source'] == 'sixents'):  sock.connect(config.sixents['url'],config.sixents['port'])
                                    elif(config.device_Mode['Alter_Source'] == 'tencent'):  sock.connect(config.tencent['url'],config.tencent['port'])
                                    # 向服务端发送消息
                                    sock.send_data("GET /%s HTTP/1.0\r\nUser-Agent:NTRIP AIR724_RTK/1.0.0\r\nAccept: */*\r\nConnection: close\r\nAuthorization: Basic %s\r\n\r\n" % (config.qxwz['MOintPoint'][0],ubinascii.b2a_base64(config.qxwz['userIP']+':'+config.qxwz['password']).decode()))
                                    sock.__nw_flag = True
                                    if(sock.rec_data().find("200 OK") != -1):
                                        print("服务器连接成功")
                                        reset_num = 0
                                        from usr.test import usart
                                        usart.stm32_usart.Queue_put("{" + "\"Name\":\"Ntrip\",\"fun\":\"Authen\",\"msg\":\"success\"" + "}")
                                        config.device_Mode['Current_Source'] = config.device_Mode['Alter_Source']
                                        config.device_Mode['Current_Mode'] = 'Moving'
                                        config.device_Mode['Alter_Source'] = ''
                                        config.device_Mode['Alter_Mode']   = ''
                                        #开启每秒回传GNGGA数据
                                        socket_time.start(1,time_cb)
                                        if( config.ble_Isopen == 1 ):
                                            #进行蓝牙消息回复
                                            config.R_WorkMode(config.device_Mode['Current_Mode'])
                                    else:
                                        print("密码出错")
                                except Exception as e:
                                    print("Ntrip鉴权失败")
                                    from usr.test import usart
                                    usart.stm32_usart.Queue_put("{" + "\"Name\":\"Ntrip\",\"fun\":\"Authen\",\"msg\":\"fail\"" + "}")
                                    if(Isreset == 0):
                                        config.reconn_fail_apply()
                                    Isreset == 0
                            else:
                                print("获取挂载点失败")
                        else:
                            print("获取挂载点失败")
                    except Exception as e:
                        print("Ntrip获取挂载点服务器申请出错")
                        from usr.test import usart
                        usart.stm32_usart.Queue_put("{" + "\"Name\":\"Ntrip\",\"fun\":\"MointPoint\",\"msg\":\"fail\"" + "}")
                        if(Isreset == 0):
                            config.reconn_fail_apply()
                        Isreset == 0
            else :
                print("SOCKET传来数据失败\r\n")
        utime.sleep_ms(10)

class socket(object):
    def __init__(self):
        #用于判断网络是否好
        self.__nw_flag = False
        # 创建基于TCP的客户端套接字(配合bind使用，可自定义socket address)
        self.sock = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)

    def connect(self,address,port):
        self.sock.connect((address,port))

    def send_data(self,bytes):
        self.sock.send(bytes)

    def rec_data(self):
        return self.sock.recv(500).decode()
    
    def rec_long_data(self):
        print(len(self.sock.recv(700)))

    def close(self):
        self.sock.close()

sock = socket()
socket_time = timer()

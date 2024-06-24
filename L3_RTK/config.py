import _thread
import utime
from misc import Power


VERSION = "1.0"

#设备IMEI
Device_IMEI = ''

#HTTP获取设备信息 
device_info = {
    'companyId': 0,
    'deviceId' : 0,
    'groupId'  : 0
}

stm32_msg = {
    'Name':'',
    
}

#蓝牙是否开启
ble_Isopen = 0

#设备模式
device_Mode={
    'Last_Mode'     : 'Moving',
    'Last_Source'   : 'qxwz',
    'Current_Mode'  : '',
    'Current_Source': '',
    'Alter_Mode'    : 'Moving',
    'Alter_Source'  : 'qxwz',
}

UART_para={
    'id'            : '', 
    'bate'          :115200, 
    'data_bits'     :8,
    'parity'        :0, 
    'stop_bits'     :1
}

IMU ={
    'ins_fre'       : 0
}

#手动输入基准点 坐标
Base_local ={
    'Base_flag'     : '2',
    'base_lat'      : '',
    'base_lon'      : '',
    'base_alt'      : ''
    }

#http
#获取设备初始信息，判断是否注册
http_info = {'url':'http://dev.server.sdxinzuobiao.com:10004/xzb/mower/hardware/authentication'}


# 差分源信息
qxwz     = {'Name':'qxwz',     'url':'rtk.ntrip.qxwz.com', 'port':8002, 'userIP':'qxxuft005', 'password':'4656f0a', 'MOintPoint':[]}   # 千寻信息
# qxwz     = {'Name':'qxwz',     'url':'rtk.ntrip.qxwz.com', 'port':8002, 'userIP':'qxxuft007', 'password':'deb9065', 'MOintPoint':[]} # 千寻信息
starcart = {'Name':'starcart', 'url':'loc.starcart.cn',    'port':2101, 'userIP':'',                 'password':'', 'MOintPoint':[]}   # 星舆信息
huacenav = {'Name':'huacenav', 'url':'rtk.huacenav.com',   'port':8002, 'userIP':'',                 'password':'', 'MOintPoint':[]}   # 华测信息
pnt      = {'Name':'pnt',      'url':'sdk.pnt.10086.cn',   'port':8002, 'userIP':'',                 'password':'', 'MOintPoint':[]}   # 移动信息
sixents  = {'Name':'sixents',  'url':'vrs.sixents.com',    'port':8002, 'userIP':'',                 'password':'', 'MOintPoint':[]}   # 六分信息
tencent  = {'Name':'tencent',  'url':'cors.tencent.com',   'port':8002, 'userIP':'',                 'password':'', 'MOintPoint':[]}   # 腾讯信息

#已建成自建基站的服务器信息
Base_MQTT      = {'client_id': 'Base_MQTT' ,     'IP':'xzb.server.sdxinzuobiao.com',  'port':'1883', 'username':'xzbL1', 'password':'2023Sd#1l!','Topic':'xzb/autopilot/common/station/861714058663760'}

#当前设备作为基站时的服务器信息
Base_self_MQTT = {'client_id': 'Base_self_MQTT' ,'IP':'222.134.70.134',               'port':'1883', 'username':'test', 'password':'XzbTest2023','Topic':'xzb/rtk/public/RTCM32_GGGB'}

#APP信息
App_MQTT       = {'client_id': 'APP_MQTT' ,      'IP':'dev.server.sdxinzuobiao.com',  'port':'1883', 'username':'wanmeng', 'password':'WanmengKeji',  'Pub_Topic': '', 'Sub_Topic': ''}

# #APP设备状态信息上报
# App_MQTT_device_status_info = {
#     "property":{
#         "obsFD": 100,
#         "status": 1001,                                                  #设备状态，1001：开机，1002待机
#         "bleMac": "c8:c2:c6:5c:78:d7",                                   #设备蓝牙MAC地址
#         "lat": 36.831173850333,                                          #gps纬度
#         "swath": 1,
#         "obsSwitch": 0,
#         "obstacle": 100,
#         "RmotTemp": 43,
#         "voltage": 0,
#         "lon": 118.077602868,                                            #gps经度
#         "current": 0,
#         "battery": 72,                                                   #电池电量，百分比
#         "cutH": 50,                                                      #割刀高度，单位mm，范围20-50
#         "obstacler": 100,
#         "time": 1685090137,                                              #时间戳，秒
#         "yaw": -0.27799999999999,                                        #设备偏航角度，浮点类型，正北为0，逆时针增加
#         "DmotTemp": 60.12,
#         "netSignal": 4,                                                  #网络信号，1-5共5个等级，5级最强
#         "obsRD": 50,
#         "obsLD": 50,
#         "gId": 7,                                                        #groupId
#         "safeS": 1,                                                      #安全区域状态，1：开，0：关闭
#         "obstaclel": 100,
#         "dId": 3,                                                        #deviceId
#         "cId": 1,                                                        #companyTag
#         "HmotTemp": 42,
#         "CmotTemp": 42,
#         "LmotTemp": 41,
#         "firmware": "1.0.32",                                            #固件版本
#         "locSignal": 5,                                                  #定位信号，1-5共5个等级，5级最强
#         "light": 0,                                                      #车灯状态，1：开，0：关
#         "cutS": 1,                                                       #割刀状态，1：开，0：关开始任务
#         "speed": "0.4"                                                   #移动速度，单位m/s
#     },
#     "task": {                                                            #任务状态
#         "pitch": 0,
#         "OffCourse": 0.0077417333048454,
#         "taskId": 110,                                                   #任务ID
#         "tStatus": 1500,                                                 #任务状态，1500：空闲，1501：作业中，1502：暂停，1503：作业完成，1504：返航中，1541：遇到障碍物停止
#         "thisdis": 0,
#         "yaw": 0,
#         "tarang": 0.97090281816185,
#         "thisang": 0.91893546517247,
#         "roll": 0,
#         "process": 1,                                                    #作业进度，0-100
#         "carang": 0,
#         "tarIndex": 1,                                                   #在作业航线中，设备当前目标行进点的序号，从0开始
#         "zoneId": 26,                                                    #作业区域ID
#         "erdelang": 1.2489028181618
#     },
#     "healthy": {                                                         #自检状态
#         "network": 3001,                                                 #网络状态，3001：正常，3120：失联，3121：无网络，3122：网络状态差
#         "bound": 3001,                                                   #安全区域状态，3001：正常，3162：超出安全区域
#         "loc": 3001,                                                     #定位状态，3001：正常，3110：失联，3111：无定位，3112：普通定位
#         "uplift": 3001,                                                  #抬起状态，3001：正常，3143110：失联，0：失联，3141：抬起
#         "battery": 3001,                                                 #电池状态，3001：正常，3150：失联，3151：电量低，3152：充电中，3153：充电完成
#         "tem": 3001                                                      #温度状态，3001：正常，3130：失联，3131温度高
#     }
# }

#是否回复当前的差分数据服务器账号密码
isreply = 1

#回复现在状态
def R_WorkMode(mode):
    global isreply
    from usr.test.ble_drive import ble_gatt_send_str_notification
    if(mode == "Base"):
        sendbf = "R" + chr(21) + \
                "1" + ","+ \
                Base_local['Base_flag'] + "," +\
                Base_local['base_lon'] + ',' + Base_local['base_lat'] + ',' + Base_local['base_alt'] + ',' +\
                '1' + ',' +\
                Base_MQTT['IP'] + ',' + Base_MQTT['port'] + ',' + Base_MQTT['username'] + ','+ Base_MQTT['password'] + ',' + Base_MQTT['Topic'] +\
                chr(0)
        ble_gatt_send_str_notification(0,sendbf)
    if(mode == "Moving"):
        orin = ''
        data = ''
        if(device_Mode['Current_Source'] != 'dif_base'):
            orin = "2"
            if(device_Mode['Current_Source'] == 'qxwz'):        
                source = "1"   
                name = qxwz['userIP']
                password = qxwz['password']
            elif(device_Mode['Current_Source'] == 'starcart'):  
                source = "2"
                name = starcart['userIP'] 
                password = starcart['password']
            elif(device_Mode['Current_Source'] == 'huacenav'):  
                source = "3"
                name = huacenav['userIP'] 
                password = huacenav['password']
            elif(device_Mode['Current_Source'] == 'pnt'):       
                source = "4"
                name = pnt['userIP'] 
                password = pnt['password']
            elif(device_Mode['Current_Source'] == 'sixents'):   
                source = "5"
                name = sixents['userIP'] 
                password = sixents['password']
            elif(device_Mode['Current_Source'] == 'tencent'):   
                source = "6"
                name = tencent['userIP'] 
                password = tencent['password']
            if(isreply == 1): data = source + ',' + name+ ',' + password + ',' + '' + ',' + ''
            else:             
                data = source + ',' + ''+ ',' + '' + ',' + '' + ',' + ''
                isreply = 1

        elif(device_Mode['Current_Source'] == 'dif_base'):
            orin = "1"
            data = Base_MQTT['IP'] + ',' + Base_MQTT['port']+ ',' + Base_MQTT['username'] + ',' + Base_MQTT['password'] + ',' + Base_MQTT['Topic']
        else :
            orin = "3"
            data = '' + ',' + '' + ',' + '' + ',' + '' + ',' + ''
        sendbf = 'R' + chr(21) + \
                '2' + ',' +\
                orin + ','+\
                data + chr(0)
        ble_gatt_send_str_notification(0,sendbf)

def R_DataProtocol():
    from usr.test.ble_drive import ble_gatt_send_str_notification
    if(UART_para['stop_bits'] == 1):
        com1par_stop = '1.0'
    else:
        com1par_stop = '2.0'
    if(UART_para['parity'] == 0):
        com1par_par = '0'
    elif(UART_para['parity'] == 1):
        com1par_par = '1'
    elif(UART_para['parity'] == 2):
        com1par_par = '2'
    sendbf = 'R' + chr(41) +\
            '1' + ',' +\
            str(UART_para['bate']) + ',' +\
            com1par_stop + ',' +\
            '8' + ',' +\
            com1par_par + chr(0)
    ble_gatt_send_str_notification(0,sendbf)
        
# 更改模式
def change_run_Mode():
    from usr.test import mqtt
    from usr.test import um482
    from usr.test import socket
    from usr.test import usart
    if(device_Mode['Current_Mode'] == 'Base'):
        print('转变为游端模式')
        device_Mode['Alter_Source'] = device_Mode['Last_Source']
        mqtt.MQTT_self_Base.disconnect()
        um482.um482.set_mode(1)
        if(device_Mode['Alter_Source'] == 'dif_base'):
            mqtt.MQTT_Base.conn_status = 0
            mqtt.MQTT_Base = mqtt.mqtt(Base_MQTT['client_id'],Base_MQTT['IP'],Base_MQTT['port'],Base_MQTT['username'],Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
            mqtt.MQTT_Base_data_semp.release()
        else:
            socket.socket_long_con = 0
            usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
            socket.sock = socket.socket()
            if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                socket.SOCKET_data_semp.release()
    if(device_Mode['Current_Mode'] == 'Moving'):
        print('转变为基站模式')
        um482.um482.set_mode(0)
        if(device_Mode['Current_Source'] != 'dif_base'):
            device_Mode['Last_Source'] = device_Mode['Current_Source']
            _thread.stop_thread(usart.SOCKET_data_apply)
            socket.socket_long_con = 0
            socket.socket_time.stop()
            socket.sock.close()
        else:
            mqtt.MQTT_Base.disconnect()
        mqtt.MQTT_self_Base = mqtt.mqtt(Base_self_MQTT['client_id'],Base_self_MQTT['IP'],Base_self_MQTT['port'],Base_self_MQTT['username'],Base_self_MQTT['password'],mqtt.MQTT_self_Base_data_semp)
        mqtt.MQTT_self_Base_data_semp.release()

#更改差分源   来自基站，也可以来自千寻等源
def change_run_source(Source):
    from usr.test import mqtt
    from usr.test import socket
    from usr.test import usart
    if(Source == '1'):    device_Mode['Alter_Source'] = 'dif_base'
    elif(Source == '2'):  device_Mode['Alter_Source'] = device_Mode['Last_Source']
    elif(Source == '3'):  device_Mode['Alter_Source'] = ''
    if(device_Mode['Current_Mode'] == 'Moving' ):
        print(device_Mode['Current_Source'])
        print("转换为：{}".format(device_Mode['Alter_Source']))
        if(device_Mode['Current_Source'] == 'dif_base' ):
            mqtt.MQTT_Base.disconnect()
            if(device_Mode['Alter_Source'] != 'dif_base'):
                socket.socket_long_con = 0
                usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                socket.sock = socket.socket()
                if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                    socket.SOCKET_data_semp.release()
            else:
                mqtt.MQTT_Base = mqtt.mqtt(Base_MQTT['client_id'],Base_MQTT['IP'],Base_MQTT['port'],Base_MQTT['username'],Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
                mqtt.MQTT_Base_data_semp.release()
        else:
            device_Mode['Last_Source'] = device_Mode['Current_Source']
            _thread.stop_thread(usart.SOCKET_data_apply)
            socket.socket_time.stop()
            socket.sock.close()
            socket.socket_long_con = 0
            if(device_Mode['Alter_Source'] == 'dif_base'):
                mqtt.MQTT_Base = mqtt.mqtt(Base_MQTT['client_id'],Base_MQTT['IP'],Base_MQTT['port'],Base_MQTT['username'],Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
                mqtt.MQTT_Base_data_semp.release()
            else:
                socket.socket_long_con = 0
                usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                socket.sock = socket.socket()
                if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                    socket.SOCKET_data_semp.release()

#设置差分账号
def change_run_base_source(Source,Name_password):
    global isreply
    from usr.test import mqtt
    from usr.test import socket
    from usr.test import usart
    print(Name_password)
    if(Name_password[0] == '' or Name_password[1] == ''):
        device_Mode['Alter_Source'] = 'qxwz'
        isreply = 0
        if(device_Mode['Current_Source'] == 'starcart'):  
            starcart['userIP']   = Name_password[0]
            starcart['password'] = Name_password[1]
        elif(device_Mode['Current_Source'] == 'huacenav'):  
            huacenav['userIP']   = Name_password[0]
            huacenav['password'] = Name_password[1]
        elif(device_Mode['Current_Source'] == 'pnt'):  
            pnt['userIP']   = Name_password[0]
            pnt['password'] = Name_password[1]
        elif(device_Mode['Current_Source'] == 'sixents'):  
            sixents['userIP']   = Name_password[0]
            sixents['password'] = Name_password[1]
        elif(device_Mode['Current_Source'] == 'tencent'):  
            tencent['userIP']   = Name_password[0]
            tencent['password'] = Name_password[1]

    else:
        if(Source == 49):    
            device_Mode['Alter_Source'] = 'qxwz'
            qxwz['userIP']   = Name_password[0]
            qxwz['password'] = Name_password[1]
        elif(Source == 50):  
            device_Mode['Alter_Source'] = 'starcart'
            starcart['userIP']   = Name_password[0]
            starcart['password'] = Name_password[1]
        elif(Source == 51):  
            device_Mode['Alter_Source'] = 'huacenav'
            huacenav['userIP']   = Name_password[0]
            huacenav['password'] = Name_password[1]
        elif(Source == 52):  
            device_Mode['Alter_Source'] = 'pnt'
            pnt['userIP']   = Name_password[0]
            pnt['password'] = Name_password[1]
        elif(Source == 53):  
            device_Mode['Alter_Source'] = 'sixents'
            sixents['userIP']   = Name_password[0]
            sixents['password'] = Name_password[1]
        elif(Source == 54):  
            device_Mode['Alter_Source'] = 'tencent'
            tencent['userIP']   = Name_password[0]
            tencent['password'] = Name_password[1]

    if(device_Mode['Current_Mode'] == 'Moving' and device_Mode['Alter_Source'] != device_Mode['Current_Source']):
        
        print(device_Mode['Current_Source'])
        print('转换为：{}'.format(device_Mode['Alter_Source']))
        if(device_Mode['Current_Source'] == 'dif_base'):
            mqtt.MQTT_Base.disconnect()
            socket.socket_long_con = 0
            usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
            socket.sock = socket.socket()
            if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                socket.SOCKET_data_semp.release()
        else:
            device_Mode['Last_Source'] = device_Mode['Current_Source']
            _thread.stop_thread(usart.SOCKET_data_apply)
            socket.socket_time.stop()
            socket.sock.close()
            socket.socket_long_con = 0
            if(device_Mode['Alter_Source'] == 'dif_base'):
                mqtt.MQTT_Base = mqtt.mqtt(Base_MQTT['client_id'],Base_MQTT['IP'],Base_MQTT['port'],Base_MQTT['username'],Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
                mqtt.MQTT_Base_data_semp.release()
            else:
                socket.socket_long_con = 0
                usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                socket.sock = socket.socket()
                if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                    socket.SOCKET_data_semp.release()
    else:  #如果进入此函数但是并未切换源，直接反馈给蓝牙
        R_WorkMode(device_Mode['Current_Mode'])

def Set_Radio():
    from usr.test import mqtt
    mqtt.MQTT_self_Base.disconnect()
    mqtt.MQTT_self_Base = mqtt.mqtt(Base_self_MQTT['client_id'],Base_self_MQTT['IP'],Base_self_MQTT['port'],Base_self_MQTT['username'],Base_self_MQTT['password'],mqtt.MQTT_self_Base_data_semp)
    mqtt.MQTT_self_Base_data_semp.release()

#恢复出厂设置             --------------------------------------------------------------------------------------------------通过32给他们重启
def reset_config():
    from usr.test.um482 import um482
    device_Mode['Current_Mode']    = 'Moving'
    device_Mode['Current_Source']  = 'qxwz'
    qxwz['userIP']                 =  ''
    qxwz['password']               =  ''

    Base_local['Base_flag']        = 2
    Base_local['base_lat']         = ''
    Base_local['base_lon']         = ''
    Base_local['base_alt']         = ''

    Base_MQTT['IP']                = ''
    Base_MQTT['port']              = ''
    Base_MQTT['username']          = ''
    Base_MQTT['password']          = ''
    Base_MQTT['Topic']             = ''

    UART_para['bate']              = 115200
    UART_para['stop_bits']         = 2
    UART_para['parity']            = 0

    um482.set_mode(1)
    utime.sleep_ms(500)
    Power.powerRestart()

# 重连失败解决方式
def reconn_fail_apply():
    from usr.test import usart
    from usr.test import mqtt
    from usr.test import socket
    if(device_Mode['Current_Mode']    == 'Base') :
        mqtt.MQTT_self_Base = mqtt.mqtt(Base_self_MQTT['client_id'],Base_self_MQTT['IP'],Base_self_MQTT['port'],Base_self_MQTT['username'],Base_self_MQTT['password'],mqtt.MQTT_self_Base_data_semp)
        mqtt.MQTT_self_Base_data_semp.release()
    else:
        if(device_Mode['Current_Source']  == 'dif_base'):
            mqtt.MQTT_Base.conn_status = 0
            mqtt.MQTT_Base = mqtt.mqtt(Base_MQTT['client_id'],Base_MQTT['IP'],Base_MQTT['port'],Base_MQTT['username'],Base_MQTT['password'],mqtt.MQTT_Base_data_semp)
            mqtt.MQTT_Base_data_semp.release()
        else:
            device_Mode['Alter_Source'] = device_Mode['Current_Source']
            device_Mode['Alter_Mode']   = device_Mode['Current_Mode']
            socket.socket_long_con = 0
            usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
            socket.sock = socket.socket()
            if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                socket.SOCKET_data_semp.release()

# 重启socket
def socket_reset():
    from usr.test import socket
    from usr.test import usart
    _thread.stop_thread(usart.SOCKET_data_apply)
    socket.socket_time.stop()
    socket.sock.close()
    socket.socket_long_con = 0
    socket.Isreset = 1
    utime.sleep(5)
    if(socket.reset_num <= 5):
        while True: #等待 网络连接成功
            if(socket.sock.__nw_flag == True):
                device_Mode['Alter_Source'] = device_Mode['Current_Source']
                device_Mode['Alter_Mode']   = device_Mode['Current_Mode']
                socket.socket_long_con = 0
                usart.SOCKET_data_apply    = _thread.start_new_thread(socket.SOCKET_data_task, ())   # 创建一个线程，当函数无参时传入空的元组
                socket.sock = socket.socket()
                if(socket.SOCKET_data_semp.getCnt().curCnt != 1):
                    socket.SOCKET_data_semp.release()
                    socket.reset_num = socket.reset_num + 1
                    break
            else:
                # 网络还未连接
                # 等待5秒钟重新获取一次
                utime.sleep(5)
    else:
        socket.reset_num = 0
        socket.socket_long_con = 0
        from usr.test import usart
        usart.stm32_usart.Queue_put("{" + "\"Name\":\"Ntrip\",\"fun\":\"connect\",\"msg\":\"recon_fail\"" + "}")
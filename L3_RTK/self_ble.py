from usr.test.timer import timer
from usr.test import um482
from usr.test import config
from usr.test import usart

import utime
import ustruct
import sim
import _thread

def ble_self_control(data_len,data):
    print('len={}, data:{}'.format(data_len, data))
    MessageType = data[0]
    if(chr(MessageType) == "C"): #----功能
        Fucsin =  data[1]
        SETCMD =  data[2]
        if(Fucsin == 21): #----设置工作模式
            if(SETCMD == 1):   config.device_Mode['Alter_Mode']  = 'Base'
            elif(SETCMD == 2): config.device_Mode['Alter_Mode']  = 'Moving'
            if(config.device_Mode['Current_Mode'] != config.device_Mode['Alter_Mode']):
                print("设置工作模式")
                config.change_run_Mode()
                
        elif(Fucsin == 22): #---设置差分来源
            config.change_run_source(chr(SETCMD))
            
        elif(Fucsin == 23): #---设置差分账号
            a_points = data[4:-1].decode()
            config.change_run_base_source(SETCMD , a_points.split(','))

        elif(Fucsin == 24):#----清除差分账号
            config.change_run_base_source('1' , ['',''])

        elif(Fucsin == 25): #---设置基站信息
            thmsg = data[2:-1].decode().split(',')
            if(config.Base_MQTT['IP'] != thmsg[0] or config.Base_MQTT['username'] != thmsg[2] or config.Base_MQTT['Topic'] != thmsg[4]):
                if(len(thmsg) == 5):
                    config.Base_MQTT['IP'] = thmsg[0]
                    config.Base_MQTT['port'] = thmsg[1]
                    config.Base_MQTT['username'] = thmsg[2]
                    config.Base_MQTT['password'] = thmsg[3]
                    config.Base_MQTT['Topic'] = thmsg[4]
                    config.change_run_source('1')
            config.R_WorkMode(config.device_Mode['Current_Mode'])
        elif(Fucsin == 26):#---nmea指令
            cdat = data[2:-1].decode()
            if(config.device_Mode['Current_Mode'] == 'Moving'):
                if(cdat.find('IMU') != -1 ):
                    imus = cdat.split(" ")
                    if(float(imus[4]) >= 0.1):
                        config.IMU['ins_fre'] = 1000 * float(imus[4])
                else :
                    um482.um482.Queue_put('{}\r\n'.format(cdat))
                    utime.sleep_ms(10)
                    um482.um482.Queue_put('saveconfig\r\n')
        elif(Fucsin == 31):#---设置基准点 --自动
            config.Base_local['Base_flag'] = chr(SETCMD)
            if (SETCMD == '2'):
                um482.um482.Queue_put("mode base time 60 1.5 2.5\r\n")
                um482.um482.Queue_put("saveconfig\r\n")
            config.R_WorkMode(config.device_Mode['Current_Mode'])
        elif(Fucsin == 32):#--设置基准点 --手动输入
            basep = data[2:-1].decode().split(',')
            config.Base_local['base_lat'] = basep[0]
            config.Base_local['base_lon'] = basep[1]
            config.Base_local['base_alt'] = basep[2]
            sendbuf = "mode base" + basep[0] + ' ' + basep[1] + ' ' + basep[2] + '\r\n'
            um482.um482.Queue_put(sendbuf)
            um482.um482.Queue_put("saveconfig\r\n")
            config.R_WorkMode(config.device_Mode['Current_Mode'])
        elif(Fucsin == 33):#--设置广播方式

            config.R_WorkMode(config.device_Mode['Current_Mode'])
        elif(Fucsin == 41):#设置串口输出参数
            usart.stm32_usart_status = 0
            _thread.stop_thread(usart.stm32_usart_send_apply)
            comp = data[2:-1].decode().split(',') #--波特率，停止位，数据位，校验位
            config.UART_para['bate'] = comp[0] 
            if(comp[3] == '1'):
                config.UART_para['parity'] = 2
            elif(comp[3] == '2'):
                config.UART_para['parity'] = 1
            else:
                config.UART_para['parity'] = 0
            if(comp[1] =="2.0"):
                config.UART_para['stop_bits'] = 2
            else:
                config.UART_para['stop_bits'] = 1
            usart.reset_uart()
            config.R_DataProtocol()
        elif(Fucsin == 42):#设置输出内容       
            print("设置输出内容")
        elif(Fucsin == 43):#恢复出厂设置
            config.reset_config()
        elif(Fucsin == 44):#请求固件升级
            print('固件升级')
        elif(Fucsin == 46):#设置USB网络共享
            print('设置USB网络共享')
        elif(Fucsin == 48):#设置网络连接方式
            print('设置网络连接方式')
        elif(Fucsin == 60): #设置WiFi参数
            print('设置wifi参数')
    elif(chr(MessageType) == "Q"):
        from usr.test.ble_drive import ble_gatt_send_str_notification
        Fucsin =  data[1]
        if(Fucsin == 21): #获取工作模式
            config.R_WorkMode(config.device_Mode['Current_Mode'])
        elif(Fucsin == 41):#获取串口配置
            config.R_DataProtocol()
        elif(Fucsin == 42):#-- 获取数据输出通道 暂无
            sendbf = 'R' + chr(42) +\
                '2' + chr(0)
            ble_gatt_send_str_notification(0,sendbf)
        elif(Fucsin == 45):#-- 获取版本
            sendbf = 'R' + chr(45) +\
                config.VERSION + chr(0)
            ble_gatt_send_str_notification(0,sendbf)
        elif(Fucsin == 46):#-- 设置USB网络共享
            print("设置USB网络共享")
        elif(Fucsin == 47):#-- 查询NMEA语句
            print("查询NMEA语句")
        elif(Fucsin == 48):#-- 查询网络连接方式
            sendbf = 'R' + chr(48) +\
                '1' + chr(0)
            ble_gatt_send_str_notification(0,sendbf)
        elif(Fucsin == 49):#-- 查询流量卡的ICCID(用于查询剩余流量)
            sendbf = 'R' + chr(49) +\
               sim.getIccid() + chr(0)
            ble_gatt_send_str_notification(0,sendbf)
        elif(Fucsin == 50):#-- 查询近n条log
            print('查询近n条log')
        elif(Fucsin == 51):#-- 查询WiFi参数
            print('查询WiFi参数')
        elif(Fucsin == 52):#-- 查询已经连接的本机IP AT+CIFSR
            print('查询已经连接的本机')

def ble_notice_data(arg):
    from usr.test.ble_drive import ble_gatt_send_notification
    buf = bytearray(70)
    ustruct.pack_into("bb",buf,0,0x4d,0x01)
    ustruct.pack_into("ddfBBfffffffB",buf,2,
        float(um482.gnss['Longitude']),
        float(um482.gnss['Latitude']),
        float(um482.gnss['Altitude']),
        int(um482.gnss['Mode']),
        int(um482.gnss['UsedStateCnt']),
    )
    ustruct.pack_into("fffffffB",buf,24,
        float(um482.gnss['Hdop']),
        1.0,
        float(um482.gnss['speed']),
        float(um482.gnss['CourseAngle']),
        1.0,
        1.0,
        1.0,
        1,
    )
    ble_gatt_send_notification(0,1,buf)

ble_notice_data_time = timer(300)
import log
import _thread
import utime
import math
import ujson

from machine import SPI

from queue import Queue
from machine import UART
from usr.test import config
from usr.test import mqtt
from usr.test.timer import timer

GPS_SPI = SPI(0,0,3)
SPI_Data_len = 5

gnss = {
    'Longitude'    :'',
    'Latitude'     :'',
    'Altitude'     :'12.4',
    'Mode'         :'0',
    'UsedStateCnt' :'0',
    "Hdop"         :'1.0',
    'CourseAngle'  :'20.2',
    'speed'        :'10.2'
}

GPS_supple = "\r\n                                                                                             "

GPS_Self_Base_recdata_status = 0

RTK_HZ = 50

#将度分格式的坐标转换为十进制度数格式
def ddmmtoddd(lng):
    if(lng != ""):
        fraction,integer = math.modf(float(lng)/100.0)
    lng = str(integer).split(".")[0] + "." + str(fraction*100/60).replace(".", "")[1:9]
    return lng

def time_cb(arg):
    global GPS_Self_Base_recdata_status
    GPS_Self_Base_time.stop()
    GPS_Self_Base_recdata_status = 0
    if(mqtt.MQTT_self_Base.conn_status == 1 ):
        if(	len(um482.GPS_Self_Base_data) > 2):
            # print(" ".join(map(str,um482.GPS_Self_Base_data[1:])))
            mqtt.MQTT_self_Base.publish(config.Base_self_MQTT['Topic'],"".join(um482.GPS_Self_Base_data[1:]))
    um482.GPS_Self_Base_data.clear()
    

def um482_data_task():
    global RTK_HZ
    while True:
        data = um482.GPS_queue.get()
        if data != None :
            um482.uartWrite(data)
        else :
            print("um482传来数据失败\r\n")
        utime.sleep_ms(RTK_HZ)

class GPS(object):
    def __init__(self,id, bate=460800, data_bits=8, parity=0, stop_bits=1,flow_control=0):
        self.uart_data_len = 0
        self.GPS_queue = Queue()
        self.uart = UART(id, bate, data_bits, parity, stop_bits, flow_control)
        self.GPS_socket = ''
        self.GPS_data   = ''
        self.GPS_Self_Base_data = []
        self.uart.set_callback(self.callback)
        
    def callback(self, para):
        global GPS_Self_Base_recdata_status
        if(0 == para[0]):
            # print(para[2])
            # print(self.uartRead(para[2]))
            # print("1")
            self.uart_data_len = para[2]
            if(config.device_Mode['Current_Mode'] == 'Moving'):
                self.GPS_data_apply()
            elif(config.device_Mode['Current_Mode'] == 'Base'):
                if(GPS_Self_Base_recdata_status == 0):
                    GPS_Self_Base_time.start(0 , time_cb)
                    GPS_Self_Base_recdata_status = 1
                self.GPS_Self_Base_data.append(self.uartRead(para[2]).decode())

    def Queue_put(self , data):
        return self.GPS_queue.put(data)

    def uartWrite(self, msg):
        self.uart.write(msg)

    def uartRead(self, len):
        utf8_msg = self.uart.read(len)
        # GPS_log.info("UartRead msg: {}".format(utf8_msg))
        return utf8_msg
    def set_mode(self,mode):
        global RTK_HZ
        if(mode == 1):
            self.Queue_put("unlog\r\n")
            utime.sleep_ms(50)
            self.Queue_put("unlog\r\n")
            utime.sleep_ms(10)
            self.Queue_put("mode rover\r\n")
            utime.sleep_ms(5)
            self.Queue_put("LOG GPGGA ONTIME {}\r\n".format(RTK_HZ/1000))
            utime.sleep_ms(5)
            self.Queue_put("LOG GPRMC ONTIME {}\r\n".format(RTK_HZ/1000))
            utime.sleep_ms(5)
            self.Queue_put("LOG GPHDT ONTIME {}\r\n".format(RTK_HZ/1000))
            utime.sleep_ms(5)
            self.Queue_put("saveconfig\r\n")
        else:
            self.Queue_put("unlog\r\n")
            utime.sleep_ms(50)
            self.Queue_put("unlog\r\n")
            utime.sleep_ms(10)
            self.Queue_put("LOG GPGGA ONTIME 1\r\n")
            utime.sleep_ms(10)
            if(config.Base_local['Base_flag'] == "2"):
                self.Queue_put("mode base time 60 1.5 2.5\r\n")
                utime.sleep_ms(5)
            elif(config.Base_local['Base_flag'] == "1"):
                self.Queue_put("mode base " + config.Base_local['base_lat'] + " " + config.Base_local['base_lon'] + " " + config.Base_local['base_alt'] + "\r\n")
                utime.sleep_ms(5)
            self.Queue_put("rtcm1006 com1 10\r\n")
            utime.sleep_ms(5)
            self.Queue_put("rtcm1033 com1 10\r\n")
            utime.sleep_ms(5)
            self.Queue_put("rtcm1074 com1 1\r\n")
            utime.sleep_ms(5)
            self.Queue_put("rtcm1124 com1 1\r\n")
            utime.sleep_ms(5)
            self.Queue_put("rtcm1084 com1 1\r\n")
            utime.sleep_ms(5)
            self.Queue_put("rtcm1094 com1 1\r\n")
            utime.sleep_ms(5)
            self.Queue_put("saveconfig\r\n")
            
    def GPS_data_apply(self):
        self.GPS_data = self.uartRead(self.uart_data_len).decode()
        # print("{}\r\n".format(self.GPS_data),'utf-8')
        GPGGA_addr = self.GPS_data.find('$GNGGA')
        GPRMC_addr = self.GPS_data.find('$GPRMC')
        GPHDT_addr = self.GPS_data.find('$GNHDT')
        # print(GPGGA_addr)
        if(GPGGA_addr != -1):
            split_data_GPGGA       = self.GPS_data[GPGGA_addr:].split(",")
            if(len(split_data_GPGGA) > 6):
                if(int(split_data_GPGGA[6]) != 0):
                    Finish_pos           = self.GPS_data.find('*')+5
                    self.GPS_socket      = self.GPS_data[GPGGA_addr:Finish_pos]
                    # 东半球为正 西半球为负
                    if(split_data_GPGGA[5] == 'W'):
                        gnss['Longitude']    = "-{}".format(ddmmtoddd(split_data_GPGGA[4]))
                    else:
                        gnss['Longitude']    = ddmmtoddd(split_data_GPGGA[4])
                    # 北半球为正 南半球为负
                    if(split_data_GPGGA[5] == 'S'):
                        gnss['Latitude']     = "-{}".format(ddmmtoddd(split_data_GPGGA[2]))
                    else:
                        gnss['Latitude']     = ddmmtoddd(split_data_GPGGA[2])
                    gnss['Altitude']     = split_data_GPGGA[9]
                    gnss['Mode']         = split_data_GPGGA[6]
                    gnss['UsedStateCnt'] = split_data_GPGGA[7]
                    gnss['Hdop']         = split_data_GPGGA[8]
        if(GPRMC_addr != -1):
            split_data_GPRMC = self.GPS_data[GPRMC_addr:].split(",")
            if(len(split_data_GPRMC) > 7):
                gnss['speed']        = split_data_GPRMC[7]
        # print(GPHDT_addr)
        if(GPHDT_addr != -1):
            split_data_GPHDT         = self.GPS_data[GPHDT_addr:].split(",")
            if(len(split_data_GPHDT) > 1):
                # print(len(split_data_GPHDT))
                # print(split_data_GPHDT)
                if(split_data_GPHDT[1] !=''):
                    gnss['CourseAngle']  = split_data_GPHDT[1]
                    if(mqtt.MQTT_APP.conn_status == 1):
                        GPS = ujson.dumps(gnss)
                        GPS_SPI.write("{:<180}".format(GPS), 180)


um482 = GPS(UART.UART1)
GPS_Self_Base_time = timer(200)

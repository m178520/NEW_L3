import utime
utime.sleep(5)

# import uos
# # 切换运行目录
# uos.chdir("/usr")

import _thread
import checkNet
import sim
import net
import sys



import dataCall

import modem

from usr.test import usart
from usr.test import config
from usr.test import mqtt
from usr.test import socket


sim_status_dict={
    0:"SIM卡不存在/被移除",
    1:"SIM已经准备好",
    2:"SIM卡已锁定，等待CHV1密码",
    3:"SIM卡已被阻拦，需要CHV1密码解锁密码",
    4:"由于SIM/USIM个性化检查失败，SIM卡被锁定",
    5:"由于PCK错误导致SIM卡被阻拦，需要MEP密码解除阻拦",
    6:"需要隐藏电话簿条目的密钥",
    7:"需要解锁隐藏密钥的编码",
    8:"SIM卡已锁定，等待CHV2密码",
    9:"SIM卡被阻拦，需要CHV2解锁密码",
    10:"由于网络个性化检查失败，SIM卡被锁定",
    11:"由于NCK不正确，SIM卡被阻拦，需要MEP解锁密码",
    12:"由于子网络锁个性化检查失败，SIM卡被锁定",
    13:"由于错误的NSCK，SIM卡被阻拦，需要MEP解锁密码",
    14:"由于服务提供商个性化检查失败，SIM卡被锁定",
    15:"由于SPCK错误，SIM卡被阻拦，需要MEP解锁密码",
    16:"由于企业个性化检查失败，SIM卡被锁定",
    17:"由于CCK不正确，SIM卡被阻止，需要MEP解锁密码",
    18:"SIM正在初始化，等待完成",
    19:"CHV1/CHV2/PIN错误",
    20:"SIM卡无效",
    21:"未知状态"
}



def netCallback(args):
    pdp = args[0]
    datacallState = args[1]
    if datacallState == 0:
        print('### network {} disconnected.'.format(pdp))
        mqtt.MQTT_Base.__nw_flag = False
        mqtt.MQTT_self_Base.__nw_flag = False
        mqtt.MQTT_APP.__nw_flag = False
        socket.sock.__nw_flag = False
        usart.stm32_usart.Queue_put("{" + "\"Name\":\"Net\",\"msg\":\"fail\"" + "}")
    elif datacallState == 1:
        print('### network {} connected.'.format(pdp))
        mqtt.MQTT_Base.__nw_flag = True
        mqtt.MQTT_self_Base.__nw_flag = True
        mqtt.MQTT_APP.__nw_flag = True
        socket.sock.__nw_flag = True
        usart.stm32_usart.Queue_put("{" + "\"Name\":\"Net\",\"msg\":\"success\"" + "}")


if __name__ == '__main__' :

    # 1、查询SIM卡状态
    sim_status = sim.getStatus()
    if sim_status not in sim_status_dict:
        print("接口返回失败")
        sys.exit()
    if sim_status != 1:
        print("Get SIM  status : {}".format(sim_status_dict[sim_status]))
        sys.exit()

    net_csqQuery = net.csqQueryPoll()
    print("信号强度 : {}".format(net_csqQuery))

    sim_status = net.getState()
    print("注册状态 : {}".format(sim_status[0][0]))

    iccid = sim.getIccid()
    if type(iccid).__name__ == 'int':
        print("Get ICCID failed !")
        sys.exit()
    # print("Get ICCID is : {}".format(iccid))

    # 2、等待注网成功
    stage, state = checkNet.waitNetworkReady(30)
    if stage == 3 and state == 1:
        print('Network connection successful.')
    else:
        print('Network connection failed, stage={}, state={}'.format(stage, state))
        sys.exit()  

    # 3、查询开机注网后的SIM卡信息
    imsi = sim.getImsi()
    if type(imsi).__name__ == 'int':
        print("Get IMSI failed !")
        sys.exit()
    # print("Get IMSI is : {}".format(imsi))

    iccid = sim.getIccid()
    if type(iccid).__name__ == 'int':
        print("Get ICCID failed !")
        sys.exit()
    # print("Get ICCID is : {}".format(iccid))
    
    phone_number= sim.getPhoneNumber()
    if type(phone_number).__name__ == 'int':
        print("Get phone_number failed !")
        sys.exit()
    # print("Get phone number is : {}".format(phone_number))
        
    # 设置热插拔
    sim.setSimDet(1, 1)
    # 设置网络状态变化回调函数
    dataCall.setCallback(netCallback)

    Imei = modem.getDevImei()
    if(Imei == -1):
        print("Get IMEI failed !")
        sys.exit()
    # print("Get IMEI is : {}".format(Imei))
    config.Device_IMEI = "{}".format(Imei)
    print(config.Device_IMEI)
    # usart.stm32_usart_send_apply = _thread.start_new_thread(usart.stm32_usart_send_task, ())   # 创建一个线程，当函数无参时传入空的元组
    # usart.stm32_usart_rec__apply = _thread.start_new_thread(usart.stm32_usart_rec_task, ())   # 创建一个线程，当函数无参时传入空的元组
    # while True :
    #     usart.stm32_usart.Queue_put("1122\r\n")
    #     utime.sleep(1)
    usart.stm32_usart.Queue_put("{" + "\"Name\":\"Imei\",\"msg\":\"{}\"".format(config.Device_IMEI) + "}")

    usart.stm32_usart_send_apply = _thread.start_new_thread(usart.stm32_usart_send_task, ())   # 创建一个线程，当函数无参时传入空的元组
    usart.stm32_usart_rec__apply = _thread.start_new_thread(usart.stm32_usart_rec_task, ())   # 创建一个线程，当函数无参时传入空的元组
    usart.MQTT_APP_data_apply  = _thread.start_new_thread(mqtt.MQTT_APP_data_task, ())   # 创建一个线程，当函数无参时传入空的元组

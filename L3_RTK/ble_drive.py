#BLE Server

import ble


from usr.test import self_ble
from usr.test import config

BLE_GATT_SYS_SERVICE = 1  # 0-删除系统默认的GAP和GATT服务  1-保留系统默认的GAP和GATT服务
_BLE_NAME = "XZB123"


event_dict = {
    'BLE_START_STATUS_IND': 0,  # ble start
    'BLE_STOP_STATUS_IND': 1,   # ble stop
    'BLE_CONNECT_IND': 16,  # ble connect
    'BLE_DISCONNECT_IND': 17,   # ble disconnect
    'BLE_UPDATE_CONN_PARAM_IND': 18,    # ble update connection parameter
    'BLE_SCAN_REPORT_IND': 19,  # ble gatt client scan and report other devices
    'BLE_GATT_MTU': 20, # ble connection mtu
    'BLE_GATT_RECV_WRITE_IND': 21, # when ble client write characteristic value or descriptor,server get the notice
    'BLE_GATT_RECV_READ_IND': 22, # when ble client read characteristic value or descriptor,server get the notice
    'BLE_GATT_RECV_NOTIFICATION_IND': 23,   # client receive notification
    'BLE_GATT_RECV_INDICATION_IND': 24, # client receive indication
    'BLE_GATT_SEND_END': 25, # server send notification,and receive send end notice
}

class EVENT(dict):
    def __getattr__(self, item):
        return self[item]

    def __setattr__(self, key, value):
        raise ValueError("{} is read-only.".format(key))


event = EVENT(event_dict)


def ble_callback(args):
    global BLE_GATT_SYS_SERVICE
    event_id = args[0]
    status = args[1]
    # # print('[ble_callback]: event_id={}, status={}'.format(event_id, status))

    if event_id == event.BLE_START_STATUS_IND:  # ble start
        if status == 0:
            # print('[callback] BLE start success.')
            # mac = ble.getPublicAddr()
            # if mac != -1 and len(mac) == 6:
            #     addr = '{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}'.format(mac[5], mac[4], mac[3], mac[2], mac[1], mac[0])
                # print('BLE public addr : {}'.format(addr))
            ret = ble_gatt_set_name()
            if ret != 0:
                ble_gatt_close()
                return
            ret = ble_gatt_set_param()
            if ret != 0:
                ble_gatt_close()
                return
            ret = ble_gatt_set_data()
            if ret != 0:
                ble_gatt_close()
                return
            ret = ble_gatt_set_rsp_data()
            if ret != 0:
                ble_gatt_close()
                return
            ble_gatt_add_service_init()
            ble_gatt_add_characteristic_init()
            ble_gatt_add_characteristic_value_init()
            ble_gatt_add_characteristic_desc_init()
            ret = ble_gatt_add_service_complete()
            if ret != 0:
                ble_gatt_close()
                return
            ret = ble_adv_start()
            if ret != 0:
                ble_gatt_close()
                return
        else:
            print('[callback] BLE start failed.')
    elif event_id == event.BLE_STOP_STATUS_IND:  # ble stop
        if status == 0:
            # print('[callback] ble stop successful.')
            # ble_status = ble.getStatus()
            # print('ble status is {}'.format(ble_status))
            ble_gatt_server_release()
        else:
            print('[callback] ble stop failed.')
    elif event_id == event.BLE_CONNECT_IND:  # ble connect
        if status == 0:
            # print('[callback] ble connect successful.')
            # connect_id = args[2]
            # addr = args[3]
            # addr_str = '{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}'.format(addr[0], addr[1], addr[2], addr[3], addr[4], addr[5])
            # print('[callback] connect_id = {}, addr = {}'.format(connect_id, addr_str))
            if ble_adv_stop() != 0:
                ble_gatt_close()
                return
            config.ble_Isopen = 1
            self_ble.ble_notice_data_time.start(1,self_ble.ble_notice_data)
        else:
            print('[callback] ble connect failed.')
    elif event_id == event.BLE_DISCONNECT_IND:  # ble disconnect
        if status == 0:
            # print('[callback] ble disconnect successful.')
            # connect_id = args[2]
            # addr = args[3]
            # addr_str = '{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}'.format(addr[0], addr[1], addr[2], addr[3], addr[4], addr[5])
            # print('[callback] connect_id = {}, addr = {}'.format(connect_id, addr_str))
            if ble_adv_start() != 0:
                ble_gatt_close()
                return
            config.ble_Isopen = 0
            self_ble.ble_notice_data_time.stop()
        else:
            print('[callback] ble disconnect failed.')
            ble_gatt_close()
            return
    elif event_id == event.BLE_UPDATE_CONN_PARAM_IND:  # ble update connection parameter
        if status == 0:
            print('[callback] ble update parameter successful.')
            # connect_id = args[2]
            # max_interval = args[3]
            # min_interval = args[4]
            # latency = args[5]
            # timeout = args[6]
            # print('[callback] connect_id={},max_interval={},min_interval={},latency={},timeout={}'.format(connect_id, max_interval, min_interval, latency, timeout))
        else:
            print('[callback] ble update parameter failed.')
            ble_gatt_close()
            return
    elif event_id == event.BLE_GATT_MTU:  # ble connection mtu
        if status == 0:
            print('[callback] ble connect mtu successful.')
            # handle = args[2]
            # ble_mtu = args[3]
            # print('[callback] handle = {:#06x}, ble_mtu = {}'.format(handle, ble_mtu))
        else:
            print('[callback] ble connect mtu failed.')
            ble_gatt_close()
            return
    elif event_id == event.BLE_GATT_RECV_WRITE_IND:
        if status == 0:
            # print('[callback] ble recv successful.')
            data_len = args[2]
            data = args[3]  # 这是一个bytearray
            # attr_handle = args[4]
            # short_uuid = args[5]
            # long_uuid = args[6]  # 这是一个bytearray
            print('len={}, data:{}'.format(data_len, data))
            # # print('attr_handle = {:#06x}'.format(attr_handle))
            # # print('short uuid = {:#06x}'.format(short_uuid))
            # # print('long uuid = {}'.format(long_uuid))
            self_ble.ble_self_control(data_len,data)
        else:
            print('[callback] ble recv failed.')
            ble_gatt_close()
            return
    elif event_id == event.BLE_GATT_RECV_READ_IND:
        if status == 0:
            # print('[callback] ble recv read successful.')
            data_len = args[2]
            data = args[3]  # 这是一个bytearray
            # attr_handle = args[4]
            # short_uuid = args[5]
            # long_uuid = args[6]  # 这是一个bytearray
            print('len={}, data:{}'.format(data_len, data))
            # print('attr_handle = {:#06x}'.format(attr_handle))
            # print('short uuid = {:#06x}'.format(short_uuid))
            # print('long uuid = {}'.format(long_uuid))
        else:
            print('[callback] ble recv read failed.')
            ble_gatt_close()
            return
    elif event_id == event.BLE_GATT_SEND_END:
        if status != 0:
            print('[callback] ble send data failed.')
            
    else:
        print('unknown event id.')


def ble_gatt_server_init(cb):
    ret = ble.serverInit(cb)
    if ret != 0:
        print('ble_gatt_server_init failed.')
        return -1
    # print('ble_gatt_server_init success.')
    return 0


def ble_gatt_server_release():
    ret = ble.serverRelease()
    if ret != 0:
        print('ble_gatt_server_release failed.')
        return -1
    # print('ble_gatt_server_release success.')
    return 0


def ble_gatt_open():
    ret = ble.gattStart()
    if ret != 0:
        print('ble_gatt_open failed.')
        return -1
    # print('ble_gatt_open success.')
    return 0


def ble_gatt_close():
    ret = ble.gattStop()
    if ret != 0:
        print('ble_gatt_close failed.')
        return -1
    # print('ble_gatt_close success.')
    return 0


def ble_gatt_set_name():
    code = 0  # utf8
    name = _BLE_NAME
    ret = ble.setLocalName(code, name)
    if ret != 0:
        print('ble_gatt_set_name failed.')
        return -1
    # print('ble_gatt_set_name success.')
    return 0


def ble_gatt_set_param():
    min_adv = 0x300
    max_adv = 0x320
    adv_type = 0  # 可连接的非定向广播,默认选择
    addr_type = 0  # 公共地址
    channel = 0x07
    filter_strategy = 0  # 处理所有设备的扫描和连接请求
    discov_mode = 2
    no_br_edr = 1
    enable_adv = 1
    ret = ble.setAdvParam(min_adv, max_adv, adv_type, addr_type, channel, filter_strategy, discov_mode, no_br_edr, enable_adv)
    if ret != 0:
        print('ble_gatt_set_param failed.')
        return -1
    # print('ble_gatt_set_param success.')
    return 0


def ble_gatt_set_data():
    adv_data = [0x02, 0x01, 0x05]
    ble_name = _BLE_NAME
    length = len(ble_name) + 1
    adv_data.append(length)
    adv_data.append(0x09)
    name_encode = ble_name.encode('UTF-8')
    for i in range(0, len(name_encode)):
        adv_data.append(name_encode[i])
    # print('set adv_data:{}'.format(adv_data))
    data = bytearray(adv_data)
    ret = ble.setAdvData(data)
    if ret != 0:
        print('ble_gatt_set_data failed.')
        return -1
    # print('ble_gatt_set_data success.')
    return 0


def ble_gatt_set_rsp_data():
    adv_data = []
    ble_name = _BLE_NAME
    length = len(ble_name) + 1
    adv_data.append(length)
    adv_data.append(0x09)
    name_encode = ble_name.encode('UTF-8')
    for i in range(0, len(name_encode)):
        adv_data.append(name_encode[i])
    # print('set adv_rsp_data:{}'.format(adv_data))
    data = bytearray(adv_data)
    ret = ble.setAdvRspData(data)
    if ret != 0:
        print('ble_gatt_set_rsp_data failed.')
        return -1
    # print('ble_gatt_set_rsp_data success.')
    return 0
"""
primary-服务类型 1表示主要服务 其他值表示次要服务 类型为整型。
server_id-服务ID 用来确定某一个服务 类型为整型。
uuid_type-uuid类型 0 - 长UUID 128bit 1 - 短UUID 16bit。类型为整型。
uuid_s-短UUID 2个字节(16bit)类型为整型 当uuid_type为0时 该值给0。
"""  
def ble_gatt_add_service_init():
    # print("初始化BLE服务")
    if ble_gatt_add_service(1,1,0xfee0) != 0:
        ble_gatt_close()
        return
"""
server_id-服务ID 用来确定某一个服务 类型为整型。
chara_id-特征ID 类型为整型。
chara_prop-特征的属性，十六进制数，可通过“或运算”同时指定多个属性，值具体含义如下表，类型为整型。
值	    含义
0x01	广播
0x02	可读
0x04	0x04 - 可写且不需要链路层应答
0x08	可写
0x10	通知
0x20	指示
0x40	认证签名写
0x80	扩展属性
uuid_type-uuid类型 0 - 长UUID 128bit 1 - 短UUID 16bit。类型为整型。
uuid_s-短UUID 2个字节 16bit 类型为整型 当uuid_type为0时该值给0。
"""
def ble_gatt_add_characteristic_init():
    # print("初始化BLE特征值")
    if ble_gatt_add_characteristic(1,2,0x04 | 0x08,0xfee1) != 0:   # 0x04 - 可写且不需要链路层应答
        ble_gatt_close()
        return
    if ble_gatt_add_characteristic(1,1,0x10 ,0xfee2) != 0:   # 0x10 通知
        ble_gatt_close()
        return
"""
server_id-服务ID 用来确定某一个服务 类型为整型。
chara_id-特征ID 类型为整型。
permission-特征值的权限 2个字节 十六进制数 可通过“或运算”同时指定多个属性 值具体含义如下表，类型为整型。
值	    含义
0x0001	可读权限
0x0002	可写权限
0x0004	读需要认证
0x0008	读需要授权
0x0010	读需要加密
0x0020	读需要授权认证
0x0040	写需要认证
0x0080	写需要授权
0x0100	写需要加密
0x0200	写需要授权认证
uuid_type-uuid类型 0 - 长UUID 128bit 1 - 短UUID 16bit。类型为整型。
uuid_s-短UUID 2个字节 16bit 类型为整型 当uuid_type为0时 该值给0。
value-特征值数据。类型为bytearray。
"""
def ble_gatt_add_characteristic_value_init():
    # print("初始化BLE特征值")
    if ble_gatt_add_characteristic_value(1,1,0x0001 | 0x0002,0xfee1) != 0:
        ble_gatt_close()
        return
    if ble_gatt_add_characteristic_value(1,2,0x0001 | 0x0002,0xfee2) != 0:
        ble_gatt_close()
        return
"""
server_id-服务ID 用来确定某一个服务 类型为整型。
chara_id-特征ID 类型为整型。
permission-特征值的权限 2个字节 十六进制数 可通过“或运算”同时指定多个属性 值具体含义如下表，类型为整型。
值	含义
0x0001	可读权限
0x0002	可写权限
0x0004	读需要认证
0x0008	读需要授权
0x0010	读需要加密
0x0020	读需要授权认证
0x0040	写需要认证
0x0080	写需要授权
0x0100	写需要加密
0x0200	写需要授权认证
uuid_type-uuid类型 0 - 长UUID 128bit 1 - 短UUID 16bit。类型为整型。
uuid_s-短UUID 2个字节 16bit 类型为整型 当uuid_type为0时 该值给0。
value-特征描述数据。类型为bytearray。
"""
def ble_gatt_add_characteristic_desc_init():
    # print("初始化BLE特征描述值")
    if ble_gatt_add_characteristic_desc(1,1,0x0001 | 0x0002,0xfee1,"  ") != 0:
        ble_gatt_close()
        return
    # print("初始化BLE特征描述值")
    if ble_gatt_add_characteristic_desc(1,1,0x0001 | 0x0002,0xfee2,"  ") != 0:
        ble_gatt_close()
        return
    

def ble_gatt_add_service(primary , server_id ,uuid_s):   
    uuid_type = 1  # 短UUID
    uuid_l = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    ret = ble.addService(primary, server_id, uuid_type, uuid_s, uuid_l)
    if ret != 0:
        print('ble_gatt_add_service failed.')
        return -1
    # print('ble_gatt_add_service success.')
    return 0

def ble_gatt_add_characteristic(server_id ,chara_id ,chara_prop ,uuid_s):
    uuid_type = 1  # 短UUID
    uuid_l = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    ret = ble.addChara(server_id, chara_id, chara_prop, uuid_type, uuid_s, uuid_l)
    if ret != 0:
        print('ble_gatt_add_characteristic failed.')
        return -1
    # print('ble_gatt_add_characteristic success.')
    return 0

def ble_gatt_add_characteristic_value(server_id ,chara_id ,permission ,uuid_s ):
    value_decode = []
    uuid_type = 1  # 短UUID
    uuid_l = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    for i in range(0, 300):
        value_decode.append(0x00)
    value = bytearray(value_decode)
    ret = ble.addCharaValue(server_id, chara_id, permission, uuid_type, uuid_s, uuid_l, value)
    if ret != 0:
        print('ble_gatt_add_characteristic_value failed.')
        return -1
    # print('ble_gatt_add_characteristic_value success.')
    return 0

def ble_gatt_add_characteristic_desc(server_id ,chara_id ,permission , uuid_s ,data):
    value_decode = []
    uuid_type = 1  # 短UUID
    uuid_l = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    data_encode = data.encode('UTF-8')
    for i in range(0, len(data_encode)):
        value_decode.append(data_encode[i])
    # print('set data:{}'.format(value_decode))
    value = bytearray(value_decode)
    ret = ble.addCharaDesc(server_id, chara_id, permission, uuid_type, uuid_s, uuid_l, value)
    if ret != 0:
        print('ble_gatt_add_characteristic_desc failed.')
        return -1
    # print('ble_gatt_add_characteristic_desc success.')
    return 0


def ble_gatt_send_Indication(conn_id,attr_num,data):
    value_decode = []
    data_encode = data.encode('UTF-8')
    for i in range(0, len(data_encode)):
        value_decode.append(data_encode[i])
    # print('send_Indication:{}'.format(value_decode))
    value = bytearray(value_decode)
    attr_handle = 16
    ret = ble.sendIndication(conn_id, attr_handle, value)
    if ret != 0:
        # print('ble_gatt_send_Indication failed.')
        return -1
    # # print('ble_gatt_send_Indication success.')
    return 0

def ble_gatt_send_notification(conn_id,attr_num,data):
    # data_test = [0x39, 0x39, 0x39, 0x39, 0x39, 0x39]  # 测试数据
    # # print('send_notification:{}'.format(data_test))
    # value = bytearray(data_test)
    # value_decode = []
    # data_encode = data.encode('UTF-8')
    # for i in range(0, len(data_encode)):
    #     value_decode.append(data_encode[i])
    # value = bytearray(value_decode)
    attr_handle = 20
    ret = ble.sendNotification(conn_id, attr_handle, data)
    if ret != 0:
        # print('ble_gatt_send_notification failed.')
        return -1
    # # print('ble_gatt_send_notification success.')
    return 0

def ble_gatt_send_str_notification(conn_id,data):
    # data_test = [0x39, 0x39, 0x39, 0x39, 0x39, 0x39]  # 测试数据
    # # print('send_notification:{}'.format(data_test))
    # value = bytearray(data_test)
    value_decode = []
    data_encode = data.encode('UTF-8')
    for i in range(0, len(data_encode)):
        value_decode.append(data_encode[i])
    value = bytearray(value_decode)
    attr_handle = 20
    ret = ble.sendNotification(conn_id, attr_handle, value)
    if ret != 0:
        # print('ble_gatt_send_notification failed.')
        return -1
    # # print('ble_gatt_send_notification success.')
    return 0


def ble_gatt_add_service_complete():
    global BLE_GATT_SYS_SERVICE
    ret = ble.addOrClearService(1, BLE_GATT_SYS_SERVICE)
    if ret != 0:
        # print('ble_gatt_add_service_complete failed.')
        return -1
    # print('ble_gatt_add_service_complete success.')
    return 0


def ble_gatt_clear_service_complete():
    global BLE_GATT_SYS_SERVICE
    ret = ble.addOrClearService(0, BLE_GATT_SYS_SERVICE)
    if ret != 0:
        # print('ble_gatt_clear_service_complete failed.')
        return -1
    # print('ble_gatt_clear_service_complete success.')
    return 0


def ble_adv_start():
    ret = ble.advStart()
    if ret != 0:
        # print('ble_adv_start failed.')
        return -1
    # print('ble_adv_start success.')
    return 0


def ble_adv_stop():
    ret = ble.advStop()
    if ret != 0:
        # print('ble_adv_stop failed.')
        return -1
    # print('ble_adv_stop success.')
    return 0


def ble_open_init():
    ret = ble_gatt_server_init(ble_callback)
    if ret == 0:
        ret = ble_gatt_open()
        if ret != 0:
            return -1
    else:
        return -1
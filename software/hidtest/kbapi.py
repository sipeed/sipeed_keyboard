import hid
import struct
import threading
import queue
import sys
import time
import cv2
import numpy as np



# define MAX_PAK_DATALEN 56
# typedef struct __packed{
#     uint8_t reportID;
#     uint8_t datalen;
#     uint16_t packageID;
#     uint32_t addr;
#     uint8_t data[MAX_PAK_DATALEN];
# }hid_data_report_t;
class keyboard_ctl:
    def __init__(self):
        self.Report_HEADER_Format = "<BBHL"
        self.totalpackageid = 0
        self.inputdatas = queue.Queue()
        self.outputdatas = queue.Queue()
        self.USB_VID = 0xffff
        self.UID_interface_number = 3
        self.hidruning = False
        self.isdeviceoen = False
        self.lock=threading.Lock()
        self.thid = threading.Thread(target=self.hidthread)

    def newpackage(self, reportid, packageid, addr, len):
        buffer = bytearray(8)
        struct.pack_into(self.Report_HEADER_Format, buffer, 0, reportid, len, packageid, addr)
        return buffer

    @property
    def getnextid(self):
        _id = self.totalpackageid
        self.totalpackageid += 1
        if self.totalpackageid > 0xffff:
            self.totalpackageid = 0
        return _id

    def getretid(self):
        _id = self.totalpackageid
        return _id

    def writepackage(self, addr, buffer: bytearray):
        packageid = self.getnextid
        package = self.newpackage(2, packageid, addr, len(buffer))
        package += buffer
        return package

    def readpackage(self, addr, len):
        packageid = self.getnextid
        package = self.newpackage(1, packageid, addr, len)
        return package

    def writedata(self, addr, buffer: bytearray):
        self.lock.acquire()
        datasize = len(buffer)
        datapoint = 0
        while (datasize > 0):
            if (datasize > 56):
                datatosent = 56
            else:
                datatosent = datasize
            data = buffer[datapoint:datapoint + datatosent]
            addr_ = addr + datapoint
            package = self.writepackage(addr_, data)
            self.outputdatas.put(package)
            wait = 50
            while wait > 0 and self.inputdatas.empty():
                time.sleep(0.001)
                wait -= 1
            if wait == 0:
                if self.lock.locked():
                    self.lock.release()
                return -1
            else:
                datain = self.inputdatas.get()
                reportid, retlen, packageid, retaddr = struct.unpack(self.Report_HEADER_Format, bytearray(datain[0:8]))
                if packageid != self.getretid():
                    # print('pkgid error')
                    if self.lock.locked():
                        self.lock.release()
                    return -2
                # else:
                    # print('succeed')
                if (retlen != datatosent):
                    if self.lock.locked():
                        self.lock.release()
                    return -3
            datasize -= datatosent
            datapoint += datatosent
        if self.lock.locked():
            self.lock.release()
        return datasize

    def readdata(self, addr, len: int):
        self.lock.acquire()
        datasize = len
        datapoint = 0
        retdata = bytearray(0)
        while (datasize > 0):
            if (datasize > 56):
                datatoget = 56
            else:
                datatoget = datasize
            addr_ = addr + datapoint
            package = self.readpackage(addr_, datatoget)
            self.outputdatas.put(package)
            wait = 50
            while wait > 0 and self.inputdatas.empty():
                time.sleep(0.001)
                wait -= 1
            if wait == 0:
                if self.lock.locked():
                    self.lock.release()
                return -1, retdata
            else:
                datain = self.inputdatas.get()
                reportid, retlen, packageid, retaddr = struct.unpack(self.Report_HEADER_Format, bytearray(datain[0:8]))
                if packageid != self.totalpackageid:
                    # print('pkgid error')
                    if self.lock.locked():
                        self.lock.release()
                    return -2, retdata
                # else:
                #     print('succeed')
                if (retlen != datatoget):
                    if self.lock.locked():
                        self.lock.release()
                    return -3, retdata
                retdata += bytearray(datain[8:8 + retlen])
            datasize -= datatoget
            datapoint += datatoget
        if self.lock.locked():
            self.lock.release()
        return datapoint, retdata

    def hidthread(self):
        print("Openning HID device with VID = 0x%X" % self.USB_VID)
        while self.hidruning:
            # print("in while")
            for dict in hid.enumerate(self.USB_VID):
                time.sleep(0.1)
                print(dict)
                if dict["interface_number"] != self.UID_interface_number:
                    continue
                dev = hid.device()

                try:
                    dev.open_path(dict['path'])
                except OSError:
                    # print(dev.error())
                    continue
                dev.set_nonblocking(True)
                if dev:
                    self.lock=threading.Lock()
                    self.outputdatas=queue.Queue()
                    self.inputdatas=queue.Queue()
                    self.isdeviceoen= True
                    try:
                        while self.hidruning:
                            if not self.outputdatas.empty():
                                dataout = self.outputdatas.get()
                                sent = dev.write(dataout)
                                # print("output:{}bytes:{}".format(sent, dataout))
                            str_in = dev.read(64)
                            if len(str_in) > 0:
                                # print("input:{}bytes:{}".format(len(str_in), str_in))
                                self.inputdatas.put(str_in)
                        dev.close()
                    except OSError:
                        print("error"*20)
                        self.isdeviceoen= False
                        break

    def init_hid_interface(self):
        if self.thid.is_alive():
            self.stop_and_wait()
            self.thid = threading.Thread(target=self.hidthread)
        self.hidruning = True
        self.thid.start()
    def stop_and_wait(self):
        self.hidruning =False
        self.isdeviceoen=False
        if self.thid.is_alive():
            self.thid.join()
        self.thid = threading.Thread(target=self.hidthread)

    def wait_for_kb(self,timeout=0):
        timesecond=timeout/1000;
        while not self.isdeviceoen:
            time.sleep(0.001)
            if(timeout!=0):
                timesecond-=0.001
                if timesecond<0:
                    return False
        return True

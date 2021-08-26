# Install python3 HID package https://pypi.org/project/hid/
import hid
import struct
import threading
import queue
import sys
import PyQt5
import time
import cv2
import numpy as np
from getlayout import getkeypos


#define MAX_PAK_DATALEN 56
# typedef struct __packed{
#     uint8_t reportID;
#     uint8_t datalen;
#     uint16_t packageID;
#     uint32_t addr;
#     uint8_t data[MAX_PAK_DATALEN];
# }hid_data_report_t;
Report_HEADER_Format = "<BBHL"
def newpackage(reportid,packageid,addr,len):
    buffer=bytearray(8)
    struct.pack_into(Report_HEADER_Format,buffer,0,reportid,len,packageid,addr)
    return buffer

totalpackageid=0
def writepackage(addr,buffer:bytearray):
    global totalpackageid
    package=newpackage(2, totalpackageid, addr, len(buffer))
    totalpackageid+=1
    package+=buffer
    return package



newpackage(1,1,1,10)

inputdatas=queue.Queue()
outputdatas=queue.Queue()

USB_VID = 0xffff
UID_interface_number=3
hidruning=True
def hidthread():
    global inputdatas
    global outputdatas
    print("Openning HID device with VID = 0x%X" % USB_VID)

    for dict in hid.enumerate(USB_VID):
        print(dict)
        if dict["interface_number"]!=UID_interface_number:
            continue;
        dev = hid.device()

        try:
            dev.open_path(dict['path'])
        except OSError:
            print(dev.error())
            continue
        dev.set_nonblocking(True);
        if dev:
            try:
                global hidruning
                while hidruning:
                    if not outputdatas.empty():
                        dataout=outputdatas.get();
                        sent=dev.write(dataout)
                        print("output:{}bytes:{}".format(sent,dataout))
                    str_in = dev.read(64)
                    if len(str_in)>0:
                        print("input:{}bytes:{}".format(len(str_in),str_in))
                        inputdatas.put(str_in)
            except OSError:
                hidruning=False
                return

thid=threading.Thread(target=hidthread)
thid.start()
# while hidruning:
#     try:
#         datain=input("set data:")
#         print(datain)
#         datain=int(datain)
#         package=writepackage(0x8000,struct.pack('I',datain))
#         outputdatas.put(package)
#     except KeyboardInterrupt:
#         hidruning=False
#         thid.join()
#         sys.exit(0)
#     except :
#         print("input error")
package=writepackage(0x8000,struct.pack('I',0))
outputdatas.put(package)
time.sleep(0.1)
package=writepackage(0x8000,struct.pack('I',8))
outputdatas.put(package)

keypos=getkeypos()
keypos[:,0]-=0.75
keypos[:,1]+=0.75
keyw=30;
keyh=1.9*4+0.75*2
print(keypos)

def play_frame(frame):
    xsize=frame.shape[1]
    ysize=frame.shape[0]
    # aspect=xsize/ysize;
    colordata=bytearray(0)
    for l in keypos:
        mapx=int(l[0]/keyw*xsize)
        mapy=int(l[1]/keyh*ysize)
        colorb=frame[mapy,mapx,0]
        colorg=frame[mapy,mapx,1]
        colorr=frame[mapy,mapx,2]
        colordata+=struct.pack('BBBB',colorb,colorg,colorr,0)
    datasize=len(colordata)
    datapoint=0;
    while(datasize>0):
        if(datasize>56):
            datatosent=56;
        else:
            datatosent=datasize
        data=colordata[datapoint:datapoint+datatosent]
        addr=0x9000+datapoint
        package=writepackage(addr,data)
        outputdatas.put(package)
        print(data)
        datasize-=datatosent
        datapoint+=datatosent


video=cv2.VideoCapture()
#video.open('badapple.mp4')
video.open('/sharehdd/Movies/钢铁侠3.mkv')


while hidruning:
    try:
        ret, frame1 = video.read()
        if(ret):
            play_frame(frame1)
            fps=video.get(cv2.CAP_PROP_FPS)
            cv2.imshow("video",frame1)
            cv2.waitKey(int(1000/fps))
        else:
            hidruning = False
            thid.join()
            sys.exit(0)
    except KeyboardInterrupt:
        hidruning=False
        thid.join()
        sys.exit(0)
    # try:
    #     datain=input("set index:")
    #     addr=int(datain)*4+0x9000
    #     datain = input("set color:")
    #     color = int(datain,base=0)
    #     package=writepackage(addr,struct.pack('I',color))
    #     outputdatas.put(package)
    # except KeyboardInterrupt:
    #     hidruning=False
    #     thid.join()
    #     sys.exit(0)
    # except :
    #     print("input error")

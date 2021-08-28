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
def readpackage(addr,len):
    global totalpackageid
    package=newpackage(1, totalpackageid, addr, len)
    totalpackageid+=1
    return package

def writedata(addr,buffer:bytearray):
    datasize=len(buffer)
    datapoint=0;
    while(datasize>0):
        if(datasize>56):
            datatosent=56;
        else:
            datatosent=datasize
        data=buffer[datapoint:datapoint+datatosent]
        addr_=addr+datapoint
        package=writepackage(addr_,data)
        outputdatas.put(package)
        wait=50
        while wait>0 and inputdatas.empty():
            time.sleep(0.001)
            wait-=1
        if wait==0:
            return -1
        else:
            datain=inputdatas.get()
            reportid,retlen,packageid,retaddr=struct.unpack(Report_HEADER_Format,bytearray(datain[0:8]))
            if packageid==totalpackageid:
                print('succeed')
            else:
                print('pkgid error')
                return -2
            if(retlen!=datatosent):
                return -3
        datasize-=datatosent
        datapoint+=datatosent
    return datasize

def readdata(addr,len:int):
    datasize=len
    datapoint=0;
    retdata=bytearray(0)
    while(datasize>0):
        if(datasize>56):
            datatoget=56;
        else:
            datatoget=datasize
        addr_=addr+datapoint
        package=readpackage(addr_,datatoget)
        outputdatas.put(package)
        wait=50
        while wait>0 and inputdatas.empty():
            time.sleep(0.001)
            wait-=1
        if wait==0:
            return -1,retdata
        else:
            datain=inputdatas.get()
            reportid,retlen,packageid,retaddr=struct.unpack(Report_HEADER_Format,bytearray(datain[0:8]))
            if packageid==totalpackageid:
                print('succeed')
            else:
                print('pkgid error')
                return -2,retdata
            if(retlen!=datatoget):
                return -3,retdata
            retdata+=bytearray(datain[8:8+retlen])
        datasize-=datatoget
        datapoint+=datatoget
    return datapoint,retdata


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
def init_hid_interface():
    global thid
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


keypos=getkeypos()
keypos[:,0]-=0.75
keypos[:,1]+=0.75
keyw=30;
keyh=1.9*4+0.75*2
kbaspect=keyw/keyh;
print(keypos)


keymapframe=np.zeros((1,1,1));

def gammafunction(r,g,b):
    return int(255*pow(r/255,1.8)),int(255*pow(g/255,1.8)),int(255*pow(b/255,1.8))

def play_frame_full(frame):
    frame=cv2.resize(frame,(128,int(keyh/keyw*128)))
    play_frame(frame)

def play_frame(frame):
    global keymapframe
    xsize=frame.shape[1]
    ysize=frame.shape[0]
    if xsize>128:
        frame=cv2.resize(frame,(128,int(ysize/xsize*128)))
    xsize = frame.shape[1]
    ysize = frame.shape[0]
    aspect=xsize/ysize;
    xoffset=yoffset=int(0)
    factor=1;
    if (aspect < kbaspect):
        factor = xsize / keyw;
        kb_vish = factor * keyh
        yoffset = int((ysize - kb_vish) / 2)
    else:
        factor = ysize / keyh;
        kb_visw = factor * keyw
        xoffset = int((xsize - kb_visw) / 2)
    if keymapframe.shape[0]!=ysize and keymapframe.shape[1]!=xsize:

        keymapframe=np.zeros((ysize,xsize,1),dtype='uint8')
        keyid=1
        for l in keypos:
            mapx = int(l[0] *factor)+xoffset
            mapy = int(l[1] *factor)+yoffset
            keymapframe=cv2.circle(keymapframe,(mapx,mapy),int(1*factor),(keyid),cv2.FILLED)
            keyid+=1;
    colordata=bytearray(0)
    keyid=1
    for l in keypos:
        # mapx=int(l[0]*factor)+xoffset
        # mapy=int(l[1]*factor)+yoffset
        mapimg=(keymapframe==keyid). squeeze()
        colors=frame[mapimg]
        colorb=np.mean(colors[:,0])
        colorg=np.mean(colors[:,1])
        colorr=np.mean(colors[:,2])
        colorr,colorg,colorb=gammafunction(colorr,colorg,colorb)
        colordata+=struct.pack('BBBB',int(colorb),int(colorg),int(colorr),0)
        keyid+=1;
    writedata(0x9000,colordata)

init_hid_interface()
#

package=writedata(0x8000,struct.pack('I',3))
time.sleep(2)

while hidruning:
    try:
        length,data=readdata(0x9000,272)
        print("readed:",length,data)
        size=100
        if(length>0):
            colors=np.frombuffer(data,dtype='uint8')
            colors=np.reshape(colors,(-1,4))
            visual=np.zeros((int(keyh*size),int(keyw*size),3),dtype='uint8')
            keyid=int(0)
            for l in keypos:
                posx=l[0]*size
                posy=l[1]*size
                color=(int(colors[keyid][0]),int(colors[keyid][1]),int(colors[keyid][2]))
                #print(color)
                visual=cv2.circle(visual,(int(posx),int(posy)),int(size*0.8),color,cv2.FILLED)
                keyid+=1
            cv2.imshow("vis",visual)
            cv2.waitKey(20)
        else:
            hidruning = False
            thid.join()
            sys.exit(0)
    except KeyboardInterrupt:
        hidruning=False
        thid.join()
        sys.exit(0)





package=writedata(0x8000,struct.pack('I',0))
time.sleep(0.1)
package=writedata(0x8000,struct.pack('I',8))

video=cv2.VideoCapture()
#video.open('badapple.mp4')
video.open('buy.mp4')
#video.open('/sharehdd/Movies/《终结者3》.RMVB')



while hidruning:
    try:
        ret, frame1 = video.read()
        if(ret):
            play_frame_full(frame1)
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

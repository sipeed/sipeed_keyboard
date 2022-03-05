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
from kbled import keyboard_led
from kbapi import keyboard_ctl

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



#
def testreadback(kb:keyboard_ctl):
    leds = keyboard_led(kb)
    leds.switchmode(3)
    while kb.hidruning:
        try:
            length,data=leds.getled()
            print("readed:",length,data)
            size=10
            if(length>0):
                visual=leds.getpreview(data,(size*leds.keyw,size*leds.keyh))
                cv2.imshow("vis",visual)
                cv2.waitKey(20)
            else:
                kb.wait_for_kb()
                leds.switchmode(3)
                continue
                kb.stop_and_wait()
                sys.exit(0)
        except KeyboardInterrupt:
            kb.stop_and_wait()
            sys.exit(0)




def testvideo(kb):
    leds = keyboard_led(kb)

    leds.switchmode(8)

    video=cv2.VideoCapture()
    video.open('badapple.mp4')
    # video.open('buy.mp4')
    #video.open('/sharehdd/Movies/《终结者3》.RMVB')



    while kb.hidruning:
        try:
            ret, frame1 = video.read()
            if(ret):
                kb.wait_for_kb()
                leds.play_frame_full(frame1)
                fps=video.get(cv2.CAP_PROP_FPS)
                cv2.imshow("video",frame1)
                # cv2.waitKey(int(1000/fps))
                cv2.waitKey(1)
            else:
                video.set(cv2.CAP_PROP_POS_FRAMES,0)
                # kb.stop_and_wait()
                # sys.exit(0)
        except KeyboardInterrupt:
            kb.stop_and_wait()
            sys.exit(0)

import math
def calcshader(code:str,led:keyboard_led,time:float):
    data=bytearray(0)
    id=0
    for l in led.keyledpos:
        globalsParameter = {'__builtins__': None,'print':print,'math':math}
        localsParameter={'x':l[0],'y':l[1],'time':time,'index':id,'kbw':led.keyw,'kbh':led.keyh,'rgb':(0,0,0)}
        try:
            exec(code,globalsParameter,localsParameter)
        except Exception as err:
            print("{}".format(err))
            t1=type(err)
            t2=type(ValueError())
            if t1!=t2:
                break
        rgb=localsParameter['rgb']
        data+=struct.pack('BBBB', max(0,min(int(rgb[0]*255),255)), max(0,min(int(rgb[1]*255),255)), max(0,min(int(rgb[2]*255),255)), 0)
    return data


shadercode='''
rx=(x-kbw/2)/kbh
ry=(y-kbh/2)/kbh
r=math.sin(rx*3.14+time)
g=math.sin(ry*3.14+time)
b=math.sin(math.sqrt(rx*rx+ry*ry)*3.14+time)
rgb=((r+1)/2,(g+1)/2,(b+1)/2)
'''

kb = keyboard_ctl()
leds=keyboard_led(kb)
# times=0
# size=30
# while True:
#     data=calcshader(shadercode,leds,times)
#     visual = leds.getpreview(data, (size * leds.keyw, size * leds.keyh))
#     cv2.imshow("vis", visual)
#     cv2.waitKey(20)
#     times+=0.02
# print(data)
kb.init_hid_interface()
while True:
    time.sleep(1)
# testvideo(kb)
# testreadback(kb)
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

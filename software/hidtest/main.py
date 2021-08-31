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


kb = keyboard_ctl()

kb.init_hid_interface()
testvideo(kb)
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

import cv2
import numpy as np
from kbapi import keyboard_ctl
import struct


class keyboard_oled:
    def __init__(self, _kb: keyboard_ctl):
        self.kb = _kb
        self.width=70
    def sendframe(self,id:int,frame):
        if(id<0 or id >3):pass
        frame = cv2.resize(frame, (70, 70), interpolation=cv2.INTER_NEAREST)
        frame=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)>127
        framedata=bytearray(2800//8)
        for x in range(self.width):
            for y in range(40):
                if(frame[y,x]):
                    i = y // 8
                    m = y % 8
                    n = 1 << m
                    framedata[i * self.width + x] |= n
        self.kb.writedata(0xd000+0x1000*id, framedata)
    def submit(self,ids:int):
        self.kb.writedata(0xc000, struct.pack('B', ids))
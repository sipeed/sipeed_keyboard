import cv2
import numpy as np
from kbapi import keyboard_ctl
import struct

def getkeypos():
    endpos = 30
    pos1 = []
    pos2 = []
    pos3 = []
    pos4 = []
    pos5 = []

    xpos = endpos
    ypos = 1.9 * 4

    leds = [];
    leds.insert(0, 2.4)
    leds.insert(0, 2.4)
    leds.insert(0, 7.2)
    leds.insert(0, 6.9)
    a = ([1.9] * 5)
    leds = a + leds

    pos1.append((xpos, ypos))

    for a in leds:
        xpos -= a
        pos1.insert(0, (xpos, ypos))

    xpos = endpos
    ypos = 1.9 * 3

    leds = [];
    leds.insert(0, 3.1)
    a = ([1.9] * 9)
    leds = a + leds
    leds.insert(0, 2.6)
    leds.insert(0, 2.6)
    leds.insert(0, 1.9)

    pos2.append((xpos, ypos))

    for a in leds:
        xpos -= a
        pos2.append((xpos, ypos))

    xpos = endpos
    ypos = 1.9 * 2

    leds = [];
    leds.insert(0, 2.6)
    a = ([1.9] * 10)
    leds = a + leds
    leds.insert(0, 3.1)
    leds.insert(0, 3.1)

    pos3.append((xpos, ypos))

    for a in leds:
        xpos -= a
        pos3.insert(0, (xpos, ypos))

    xpos = endpos
    ypos = 1.9 * 1

    leds = [];
    leds.insert(0, 2.4)
    a = ([1.9] * 11)
    leds = a + leds
    leds.insert(0, 2.4)
    leds.insert(0, 2.4)

    pos4.append((xpos, ypos))

    for a in leds:
        xpos -= a
        pos4.append((xpos, ypos))

    xpos = endpos
    ypos = 1.9 * 0

    leds = [];
    a = ([1.9] * 12)
    leds = a + leds
    leds.insert(0, 2.85)
    leds.insert(0, 2.85)

    pos5.append((xpos, ypos))

    for a in leds:
        xpos -= a
        pos5.insert(0, (xpos, ypos))

    # print("keys:",len(pos),pos)
    #
    # mat=numpy.zeros((400,400,3))
    # for l in pos:
    #     mat=cv2.circle(mat,(int(l[0]*10+50),int(l[1]*10+50)),5,(255,255,255))
    # cv2.imshow("keys",mat)
    # cv2.waitKey(0)
    pos = pos1 + pos2 + pos3 + pos4 + pos5
    pos = np.array(pos)
    return pos
    # print(pos)


class keyboard_led:
    def __init__(self, _kb: keyboard_ctl):
        self.keyledpos = getkeypos()
        self.keyledpos[:, 0] -= 0.75
        self.keyledpos[:, 1] += 0.75
        self.keyw = 30;
        self.keyh = 1.9 * 4 + 0.75 * 2
        self.kbaspect = self.keyw / self.keyh
        self.kb = _kb
        self.keymapframe = np.zeros((1, 1, 1))

    def gammafunction(self, r, g, b):
        return r,g,b
        return int(255 * pow(r / 255, 1.8)), int(255 * pow(g / 255, 1.8)), int(255 * pow(b / 255, 1.8))

    def play_frame_full(self, frame):
        frame = cv2.resize(frame, (128, int(self.keyh / self.keyw * 128)), interpolation=cv2.INTER_NEAREST)
        return self.play_frame(frame)

    def play_frame(self, frame):
        xsize = frame.shape[1]
        ysize = frame.shape[0]
        if xsize > 128:
            frame = cv2.resize(frame, (128, int(ysize / xsize * 128)), interpolation=cv2.INTER_NEAREST)
        xsize = frame.shape[1]
        ysize = frame.shape[0]
        aspect = xsize / ysize
        xoffset = yoffset = int(0)
        factor = 1
        if (aspect < self.kbaspect):
            factor = xsize / self.keyw
            kb_vish = factor * self.keyh
            yoffset = int((ysize - kb_vish) / 2)
        else:
            factor = ysize / self.keyh
            kb_visw = factor * self.keyw
            xoffset = int((xsize - kb_visw) / 2)
        if self.keymapframe.shape[0] != ysize and self.keymapframe.shape[1] != xsize:

            self.keymapframe = np.zeros((ysize, xsize, 1), dtype='uint8')
            keyid = 1
            for l in self.keyledpos:
                mapx = int(l[0] * factor) + xoffset
                mapy = int(l[1] * factor) + yoffset
                self.keymapframe = cv2.circle(self.keymapframe, (mapx, mapy), int(1 * factor), (keyid), cv2.FILLED)
                keyid += 1
        colordata = bytearray(0)
        keyid = 1
        for l in self.keyledpos:
            # mapx=int(l[0]*factor)+xoffset
            # mapy=int(l[1]*factor)+yoffset
            mapimg = (self.keymapframe == keyid).squeeze()
            colors = frame[mapimg]
            colorb = np.mean(colors[:, 0])
            colorg = np.mean(colors[:, 1])
            colorr = np.mean(colors[:, 2])
            colorr, colorg, colorb = self.gammafunction(colorr, colorg, colorb)
            colordata += struct.pack('BBBB', int(colorb), int(colorg), int(colorr), 0)
            keyid += 1
        self.kb.writedata(0x9000, colordata)
        return colordata
    def switchmode(self,mode:int):
        self.kb.writedata(0x8000, struct.pack('I', mode))
    def getled(self):
        length, data = self.kb.readdata(0x9000, 272)
        return length,data
    def getpreview(self,data,size):
        colors = np.frombuffer(data, dtype='uint8')
        colors = np.reshape(colors, (-1, 4))
        visual = np.zeros((int(size[1]), int(size[0]), 3), dtype='uint8')
        keyid = int(0)
        for l in self.keyledpos:
            posx = l[0] * size[0]/self.keyw
            posy = l[1] * size[1]/self.keyh
            color = (int(colors[keyid][0]), int(colors[keyid][1]), int(colors[keyid][2]))
            # print(color)
            visual = cv2.circle(visual, (int(posx), int(posy)), int(min(size[0]/self.keyw,size[1]/self.keyh)), color, cv2.FILLED)
            keyid += 1
        return visual
    def getpreview_rgb(self,data,size):
        colors = np.frombuffer(data, dtype='uint8')
        colors = np.reshape(colors, (-1, 4))
        visual = np.zeros((int(size[1]), int(size[0]), 3), dtype='uint8')
        keyid = int(0)
        for l in self.keyledpos:
            posx = l[0] * size[0]/self.keyw
            posy = l[1] * size[1]/self.keyh
            color = (int(colors[keyid][2]), int(colors[keyid][1]), int(colors[keyid][0]))
            # print(color)
            visual = cv2.circle(visual, (int(posx), int(posy)), int(min(size[0]/self.keyw,size[1]/self.keyh)), color, cv2.FILLED)
            keyid += 1
        return visual
from mainwin import *
from PyQt5.QtCore import (QByteArray, QDataStream, QFile, QFileInfo,
                          QIODevice, QPoint, QPointF, QRectF, Qt, QTimer)
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog,QGraphicsScene,QGraphicsPixmapItem,QGraphicsItem,QStyle,QGraphicsTextItem,QMenu
from PyQt5.QtGui import QImage,QPen,QBrush,QTransform,QColor
from PyQt5.QtGui import QPixmap

import os
import struct
import math
import sys
import threading
import cv2
import numpy as np
import copy

from kbled import keyboard_led
from kbapi import keyboard_ctl

class My_Application(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.tabWidget.setCurrentIndex(0)
        self.kb = keyboard_ctl()
        self.leds=keyboard_led(self.kb)
        self.video_opened=False
        self.video_running=False
        self.shader_running=False
        self.leddata=bytearray(272)

        self.ui.mode_set_btn.clicked.connect(self.set_mode_func)
        self.ui.connect_btn.clicked.connect(self.connectfunc)
        self.ui.stop_btn.clicked.connect(self.stopfunc)
        self.ui.video_play_btn.clicked.connect(self.videofunc)
        self.ui.shader_play_btn.clicked.connect(self.startshaderfunc)
        self.ui.browsebtn.clicked.connect(self.browseforvideo)
        self.ui.shaderTextbox.textChanged.connect(self.shaderchangedfunc)

        self.videothreadid = threading.Thread(target=self.videothread)
        self.shaderthreadid = threading.Thread(target=self.shaderthread)

        self.videolock=threading.Lock()
        self.shaderlock=threading.Lock()

        self.previewtimer=QTimer()
        self.previewtimer.timeout.connect(self.updateperview)
        self.videotimer=QTimer()
        self.videotimer.timeout.connect(self.videotimerfunc)
        self.shadertimer=QTimer()
        self.shadertimer.timeout.connect(self.shadertimerfunc)


        if os.path.exists("shader.txt"):
            f=open("shader.txt",'r')
            f.seek(0,os.SEEK_END)
            filesize=f.tell()
            f.seek(0,os.SEEK_SET)
            self.shadertext=f.read(filesize)
            f.close()
        else:
            self.shadertext='''
rx=(x-kbw/2)/kbh
ry=(y-kbh/2)/kbh
r=math.sin(rx*3.14+time)
g=math.sin(ry*3.14+time)
b=math.sin(math.sqrt(rx*rx+ry*ry)*3.14+time)
rgb=((r+1)/2,(g+1)/2,(b+1)/2)
'''
            f=open("shader.txt",'w')
            f.write(self.shadertext)
            f.close()
        self.ui.shaderTextbox.setText(self.shadertext)
    def shaderchangedfunc(self):
        self.shadertext=self.ui.shaderTextbox.toPlainText()
    def connectfunc(self):
        self.kb.init_hid_interface()
        if(self.kb.wait_for_kb(1000)==False):
            self.ui.logbox.appendPlainText("Connect Failed\n")
            self.kb.stop_and_wait()
            return
        self.startTimer()
    def stopfunc(self):
        self.stopTimer()
        self.stopvideo()
        self.stopshader()
        self.kb.stop_and_wait()

    def closeEvent(self, event):
        self.kb.stop_and_wait()
        self.stopvideo()
        self.stopshader()
        f = open("shader.txt", 'w')
        f.write(self.shadertext)
        f.close()
    def set_mode_func(self):
        if self.kb.isdeviceoen==False:
            return
        try:
            mode=int(self.ui.modebox.text())
        except:
            mode=0
        self.leds.switchmode(mode)

    def startTimer(self):
        self.previewtimer.start(100)
    def stopTimer(self):
        self.previewtimer.stop()
    def updateperview(self):
        if(self.kb.wait_for_kb(1000)==False):return
        if(self.video_running==False and self.shader_running==False):
            lens,self.leddata=self.leds.getled()
            if(lens<0):
                return
        width=(self.ui.preview_box.width()-10)
        height=(self.ui.preview_box.height()-10)
        img=self.leds.getpreview_rgb(self.leddata,(width,height))
        previewimg = QImage(img,
                              width, height, width*3,
                              QImage.Format_RGB888)
        pix = QPixmap.fromImage(previewimg)
        self.preitem = QGraphicsPixmapItem(pix)
        self.preitem.setScale(1)
        self.prescene = QGraphicsScene()
        self.prescene.addItem(self.preitem)
        self.ui.preview_box.setScene(self.prescene)

    def videofunc(self):
        if self.shader_running:
            self.stopshader()
        if self.kb.wait_for_kb(100)==False:
            return
        if self.videothreadid.is_alive():
            self.video_running=False
            self.videothreadid.join()
        self.videolink=self.ui.video_link_box.text()
        self.cvvideo=cv2.VideoCapture()
        self.cvvideo.open(self.videolink)
        if(self.cvvideo.isOpened()):
            self.leds.switchmode(0xff)
            fps=self.cvvideo.get(cv2.CAP_PROP_FPS)
            self.videotimer.start(int(np.floor(1000/fps)))
            self.video_opened=True
            self.video_running=True
            self.videothreadid = threading.Thread(target=self.videothread)
            self.videothreadid.start()
    def videotimerfunc(self):
        if(self.videolock.locked()):
            self.videolock.release()
    def shadertimerfunc(self):
        if(self.shaderlock.locked()):
            self.shaderlock.release()
    def videothread(self):
        kbresetflag=True
        while self.video_running and self.video_opened:
            self.videolock.acquire()
            if kbresetflag:
                kbresetflag=False
                self.leds.switchmode(0xff)
            ret, frame1 = self.cvvideo.read()
            if(ret):
                if self.kb.wait_for_kb(100) == False:
                    kbresetflag=True
                    continue
                self.leddata=self.leds.play_frame_full(frame1)
            else:
                self.cvvideo.set(cv2.CAP_PROP_POS_FRAMES,0)
                # kb.stop_and_wait()
                # sys.exit(0)
    def stopvideo(self):
        if self.videothreadid.is_alive():
            self.video_running=False
            if self.videolock.locked():
                self.videolock.release()
            self.videothreadid.join()
            self.cvvideo.release()
            self.video_opened=False
            self.videotimer.stop()
    def stopshader(self):
        if self.shaderthreadid.is_alive():
            self.shader_running=False
            if self.shaderlock.locked():
                self.shaderlock.release()
            self.shaderthreadid.join()
            self.shadertimer.stop()
    def calcshader(self,code:str,led:keyboard_led,time:float):
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
            id+=1
        return data
    def shaderthread(self):
        shadercode=self.shadertext
        t=0
        kbresetflag=True
        while self.shader_running:
            t+=1/60
            self.shaderlock.acquire()
            if kbresetflag:
                kbresetflag=False
                self.leds.switchmode(0xff)
            data = self.calcshader(shadercode,self.leds,t)
            if(len(data)==272):
                if self.kb.wait_for_kb(100) == False:
                    kbresetflag=True
                    continue
                self.leddata=data
                self.kb.writedata(0x9000,data)
            else:
                self.shader_running=False
                return
                # kb.stop_and_wait()
                # sys.exit(0)
    def startshaderfunc(self):
        if self.video_running:
            self.stopvideo()
        if self.kb.wait_for_kb(100) == False:
            return
        if self.shaderthreadid.is_alive():
            self.shader_running = False
            self.shaderthreadid.join()
        self.shadertimer.start(int(np.floor(1000 / 60)))
        self.shader_running = True
        self.shaderthreadid = threading.Thread(target=self.shaderthread)
        self.shaderthreadid.start()
    def browseforvideo(self):
        fileName_choose, filetype = QFileDialog.getOpenFileName(self,
                                                                "选取文件",
                                                                # "/sharehdd/subject2Bag/",
                                                                os.path.curdir,  # 起始路径
                                                                "All Files (*.*)")
        self.ui.video_link_box.setText(fileName_choose)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    class_instance = My_Application()
    class_instance.show()
    sys.exit(app.exec_())
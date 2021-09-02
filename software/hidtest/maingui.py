from mainwin import *
from PyQt5.QtCore import (QByteArray, QDataStream, QFile, QFileInfo,
                          QIODevice, QPoint, QPointF, QRectF, Qt, QTimer)
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog,QGraphicsScene,QGraphicsPixmapItem,QGraphicsItem,QStyle,QGraphicsTextItem,QMenu
from PyQt5.QtGui import QImage,QPen,QBrush,QTransform,QColor
from PyQt5.QtGui import QPixmap

import sys
import cv2
import numpy as np
import threading

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
        self.leddata=bytearray(272)

        self.ui.mode_set_btn.clicked.connect(self.set_mode_func)
        self.ui.connect_btn.clicked.connect(self.connectfunc)
        self.ui.stop_btn.clicked.connect(self.stopfunc)
        self.ui.video_play_btn.clicked.connect(self.videofunc)

        self.videothreadid = threading.Thread(target=self.videothread)

        self.videolock=threading.Lock()

        self.previewtimer=QTimer()
        self.previewtimer.timeout.connect(self.updateperview)
        self.videotimer=QTimer()
        self.videotimer.timeout.connect(self.videotimerfunc)

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
        self.kb.stop_and_wait()

    def closeEvent(self, event):
        self.kb.stop_and_wait()
        self.stopvideo()

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
        if(self.video_running==False):
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
    def videothread(self):
        kbresetflag=False
        while self.video_running and self.video_opened:
            self.videolock.acquire()
            if kbresetflag:
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
            self.videolock.release()
            self.videothreadid.join()
            self.cvvideo.release()
            self.video_opened=False
            self.videotimer.stop()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    class_instance = My_Application()
    class_instance.show()
    sys.exit(app.exec_())
from mainwin import *
from PyQt5.QtCore import (QByteArray, QDataStream, QFile, QFileInfo,
                          QIODevice, QPoint, QPointF, QRectF, Qt, QTimer)
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog,QGraphicsScene,QGraphicsPixmapItem,QGraphicsItem,QStyle,QGraphicsTextItem,QMenu
from PyQt5.QtGui import QImage,QPen,QBrush,QTransform,QColor
from PyQt5.QtGui import QPixmap

import sys
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

        self.ui.mode_set_btn.clicked.connect(self.set_mode_func)
        self.ui.connect_btn.clicked.connect(self.connectfunc)
        self.ui.stop_btn.clicked.connect(self.stopfunc)

        self.previewtimer=QTimer()
        self.previewtimer.timeout.connect(self.updateperview)
    def connectfunc(self):
        self.kb.init_hid_interface()
        if(self.kb.wait_for_kb(1000)==False):
            self.ui.logbox.appendPlainText("Connect Failed\n")
            return
        self.startTimer()
    def stopfunc(self):
        self.stopTimer()
        self.kb.stop_and_wait()

    def closeEvent(self, event):
        self.kb.stop_and_wait()
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
        lens,data=self.leds.getled()
        if(lens<0):return
        width=(self.ui.preview_box.width()-10)
        height=(self.ui.preview_box.height()-10)
        img=self.leds.getpreview_rgb(data,(width,height))
        previewimg = QImage(img,
                              width, height, width*3,
                              QImage.Format_RGB888)
        pix = QPixmap.fromImage(previewimg)
        self.preitem = QGraphicsPixmapItem(pix)
        self.preitem.setScale(1)
        self.prescene = QGraphicsScene()
        self.prescene.addItem(self.preitem)
        self.ui.preview_box.setScene(self.prescene)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    class_instance = My_Application()
    class_instance.show()
    sys.exit(app.exec_())
# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'main.ui'
##
## Created by: Qt User Interface Compiler version 6.2.3
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QGraphicsView, QGridLayout, QLCDNumber,
    QLabel, QLineEdit, QMainWindow, QMenu,
    QMenuBar, QPushButton, QSizePolicy, QStatusBar,
    QTabWidget, QTextEdit, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(569, 444)
        self.actionAbout = QAction(MainWindow)
        self.actionAbout.setObjectName(u"actionAbout")
        self.actionExit = QAction(MainWindow)
        self.actionExit.setObjectName(u"actionExit")
        self.actionRunning_in_Background = QAction(MainWindow)
        self.actionRunning_in_Background.setObjectName(u"actionRunning_in_Background")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.gridLayout = QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.stop_btn = QPushButton(self.centralwidget)
        self.stop_btn.setObjectName(u"stop_btn")

        self.gridLayout.addWidget(self.stop_btn, 5, 1, 1, 1)

        self.connect_btn = QPushButton(self.centralwidget)
        self.connect_btn.setObjectName(u"connect_btn")
        self.connect_btn.setMinimumSize(QSize(100, 0))
        self.connect_btn.setMaximumSize(QSize(100, 16777215))

        self.gridLayout.addWidget(self.connect_btn, 4, 1, 1, 1)

        self.label_6 = QLabel(self.centralwidget)
        self.label_6.setObjectName(u"label_6")

        self.gridLayout.addWidget(self.label_6, 3, 0, 1, 1)

        self.preview_box = QGraphicsView(self.centralwidget)
        self.preview_box.setObjectName(u"preview_box")
        self.preview_box.setMinimumSize(QSize(300, 100))

        self.gridLayout.addWidget(self.preview_box, 4, 0, 2, 1)

        self.label = QLabel(self.centralwidget)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 0, 0, 3, 2)

        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.builtin_tab = QWidget()
        self.builtin_tab.setObjectName(u"builtin_tab")
        self.gridLayout_2 = QGridLayout(self.builtin_tab)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.previous_mode = QPushButton(self.builtin_tab)
        self.previous_mode.setObjectName(u"previous_mode")

        self.gridLayout_2.addWidget(self.previous_mode, 1, 1, 1, 1)

        self.next_mode = QPushButton(self.builtin_tab)
        self.next_mode.setObjectName(u"next_mode")

        self.gridLayout_2.addWidget(self.next_mode, 1, 2, 1, 1)

        self.mode_set_btn = QPushButton(self.builtin_tab)
        self.mode_set_btn.setObjectName(u"mode_set_btn")

        self.gridLayout_2.addWidget(self.mode_set_btn, 2, 2, 1, 1)

        self.mode_id = QLCDNumber(self.builtin_tab)
        self.mode_id.setObjectName(u"mode_id")
        self.mode_id.setDigitCount(3)
        self.mode_id.setProperty("intValue", 1)

        self.gridLayout_2.addWidget(self.mode_id, 0, 2, 1, 1)

        self.label_3 = QLabel(self.builtin_tab)
        self.label_3.setObjectName(u"label_3")
        font = QFont()
        font.setPointSize(30)
        self.label_3.setFont(font)
        self.label_3.setAlignment(Qt.AlignCenter)

        self.gridLayout_2.addWidget(self.label_3, 0, 1, 1, 1)

        self.tabWidget.addTab(self.builtin_tab, "")
        self.video_tab = QWidget()
        self.video_tab.setObjectName(u"video_tab")
        self.gridLayout_3 = QGridLayout(self.video_tab)
        self.gridLayout_3.setObjectName(u"gridLayout_3")
        self.video_play_btn = QPushButton(self.video_tab)
        self.video_play_btn.setObjectName(u"video_play_btn")

        self.gridLayout_3.addWidget(self.video_play_btn, 3, 2, 1, 1)

        self.label_2 = QLabel(self.video_tab)
        self.label_2.setObjectName(u"label_2")
        self.label_2.setMinimumSize(QSize(0, 32))
        self.label_2.setMaximumSize(QSize(16777215, 32))

        self.gridLayout_3.addWidget(self.label_2, 0, 0, 1, 1)

        self.video_link_box = QLineEdit(self.video_tab)
        self.video_link_box.setObjectName(u"video_link_box")

        self.gridLayout_3.addWidget(self.video_link_box, 3, 0, 1, 1)

        self.browsebtn = QPushButton(self.video_tab)
        self.browsebtn.setObjectName(u"browsebtn")

        self.gridLayout_3.addWidget(self.browsebtn, 3, 1, 1, 1)

        self.tabWidget.addTab(self.video_tab, "")
        self.shader_tab = QWidget()
        self.shader_tab.setObjectName(u"shader_tab")
        self.gridLayout_4 = QGridLayout(self.shader_tab)
        self.gridLayout_4.setObjectName(u"gridLayout_4")
        self.shaderTextbox = QTextEdit(self.shader_tab)
        self.shaderTextbox.setObjectName(u"shaderTextbox")
        sizePolicy = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.shaderTextbox.sizePolicy().hasHeightForWidth())
        self.shaderTextbox.setSizePolicy(sizePolicy)

        self.gridLayout_4.addWidget(self.shaderTextbox, 1, 0, 4, 1)

        self.shader_play_btn = QPushButton(self.shader_tab)
        self.shader_play_btn.setObjectName(u"shader_play_btn")

        self.gridLayout_4.addWidget(self.shader_play_btn, 2, 1, 1, 1)

        self.label_5 = QLabel(self.shader_tab)
        self.label_5.setObjectName(u"label_5")
        sizePolicy1 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_5.sizePolicy().hasHeightForWidth())
        self.label_5.setSizePolicy(sizePolicy1)

        self.gridLayout_4.addWidget(self.label_5, 0, 0, 1, 1)

        self.tabWidget.addTab(self.shader_tab, "")
        self.oled_tab = QWidget()
        self.oled_tab.setObjectName(u"oled_tab")
        self.oled_key = QTabWidget(self.oled_tab)
        self.oled_key.setObjectName(u"oled_key")
        self.oled_key.setGeometry(QRect(0, 0, 541, 191))
        self.key1_tab = QWidget()
        self.key1_tab.setObjectName(u"key1_tab")
        self.key_1 = QPushButton(self.key1_tab)
        self.key_1.setObjectName(u"key_1")
        self.key_1.setGeometry(QRect(310, 70, 80, 30))
        self.key_1.setMinimumSize(QSize(80, 30))
        self.key_1.setMaximumSize(QSize(80, 30))
        self.img_b_1 = QPushButton(self.key1_tab)
        self.img_b_1.setObjectName(u"img_b_1")
        self.img_b_1.setGeometry(QRect(150, 70, 80, 30))
        self.img_b_1.setMinimumSize(QSize(80, 30))
        self.img_b_1.setMaximumSize(QSize(80, 30))
        self.oled_preview_1 = QLabel(self.key1_tab)
        self.oled_preview_1.setObjectName(u"oled_preview_1")
        self.oled_preview_1.setGeometry(QRect(30, 19, 70, 41))
        self.oled_preview_1.setAlignment(Qt.AlignCenter)
        self.key_1_text = QLineEdit(self.key1_tab)
        self.key_1_text.setObjectName(u"key_1_text")
        self.key_1_text.setGeometry(QRect(150, 30, 151, 25))
        self.key_combo_1 = QLabel(self.key1_tab)
        self.key_combo_1.setObjectName(u"key_combo_1")
        self.key_combo_1.setGeometry(QRect(30, 60, 121, 31))
        self.label_12 = QLabel(self.key1_tab)
        self.label_12.setObjectName(u"label_12")
        self.label_12.setGeometry(QRect(120, 30, 21, 17))
        self.key_1_text_set = QPushButton(self.key1_tab)
        self.key_1_text_set.setObjectName(u"key_1_text_set")
        self.key_1_text_set.setGeometry(QRect(390, 70, 80, 30))
        self.key_1_text_set.setMinimumSize(QSize(80, 30))
        self.key_1_text_set.setMaximumSize(QSize(80, 30))
        self.key_1_c_set = QPushButton(self.key1_tab)
        self.key_1_c_set.setObjectName(u"key_1_c_set")
        self.key_1_c_set.setGeometry(QRect(230, 70, 80, 30))
        self.key_1_c_set.setMinimumSize(QSize(80, 30))
        self.key_1_c_set.setMaximumSize(QSize(80, 30))
        self.oled_key.addTab(self.key1_tab, "")
        self.key2_tab = QWidget()
        self.key2_tab.setObjectName(u"key2_tab")
        self.key_combo_2 = QLabel(self.key2_tab)
        self.key_combo_2.setObjectName(u"key_combo_2")
        self.key_combo_2.setGeometry(QRect(30, 60, 121, 31))
        self.key_2_c_set = QPushButton(self.key2_tab)
        self.key_2_c_set.setObjectName(u"key_2_c_set")
        self.key_2_c_set.setGeometry(QRect(230, 70, 80, 30))
        self.key_2_c_set.setMinimumSize(QSize(80, 30))
        self.key_2_c_set.setMaximumSize(QSize(80, 30))
        self.oled_preview_2 = QLabel(self.key2_tab)
        self.oled_preview_2.setObjectName(u"oled_preview_2")
        self.oled_preview_2.setGeometry(QRect(30, 19, 70, 41))
        self.oled_preview_2.setAlignment(Qt.AlignCenter)
        self.key_2_text_set = QPushButton(self.key2_tab)
        self.key_2_text_set.setObjectName(u"key_2_text_set")
        self.key_2_text_set.setGeometry(QRect(390, 70, 80, 30))
        self.key_2_text_set.setMinimumSize(QSize(80, 30))
        self.key_2_text_set.setMaximumSize(QSize(80, 30))
        self.label_13 = QLabel(self.key2_tab)
        self.label_13.setObjectName(u"label_13")
        self.label_13.setGeometry(QRect(120, 30, 21, 17))
        self.img_b_2 = QPushButton(self.key2_tab)
        self.img_b_2.setObjectName(u"img_b_2")
        self.img_b_2.setGeometry(QRect(150, 70, 80, 30))
        self.img_b_2.setMinimumSize(QSize(80, 30))
        self.img_b_2.setMaximumSize(QSize(80, 30))
        self.key_1_text_2 = QLineEdit(self.key2_tab)
        self.key_1_text_2.setObjectName(u"key_1_text_2")
        self.key_1_text_2.setGeometry(QRect(150, 30, 151, 25))
        self.key_2 = QPushButton(self.key2_tab)
        self.key_2.setObjectName(u"key_2")
        self.key_2.setGeometry(QRect(310, 70, 80, 30))
        self.key_2.setMinimumSize(QSize(80, 30))
        self.key_2.setMaximumSize(QSize(80, 30))
        self.oled_key.addTab(self.key2_tab, "")
        self.tabWidget.addTab(self.oled_tab, "")

        self.gridLayout.addWidget(self.tabWidget, 6, 0, 1, 2)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 569, 27))
        self.menuProgram = QMenu(self.menubar)
        self.menuProgram.setObjectName(u"menuProgram")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuProgram.menuAction())
        self.menuProgram.addAction(self.actionAbout)
        self.menuProgram.addAction(self.actionExit)

        self.retranslateUi(MainWindow)

        self.tabWidget.setCurrentIndex(0)
        self.oled_key.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"SMK Manager", None))
        self.actionAbout.setText(QCoreApplication.translate("MainWindow", u"About", None))
        self.actionExit.setText(QCoreApplication.translate("MainWindow", u"Exit", None))
        self.actionRunning_in_Background.setText(QCoreApplication.translate("MainWindow", u"Running in Background", None))
        self.stop_btn.setText(QCoreApplication.translate("MainWindow", u"Stop", None))
        self.connect_btn.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.label_6.setText(QCoreApplication.translate("MainWindow", u"Preview:", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"Keyboard Control Panel", None))
        self.previous_mode.setText(QCoreApplication.translate("MainWindow", u"Previous", None))
        self.next_mode.setText(QCoreApplication.translate("MainWindow", u"Next", None))
        self.mode_set_btn.setText(QCoreApplication.translate("MainWindow", u"Set", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Mode", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.builtin_tab), QCoreApplication.translate("MainWindow", u"Built-in", None))
        self.video_play_btn.setText(QCoreApplication.translate("MainWindow", u"Play", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"Video File Path or Stream link:", None))
        self.browsebtn.setText(QCoreApplication.translate("MainWindow", u"Browse...", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.video_tab), QCoreApplication.translate("MainWindow", u"Video", None))
        self.shader_play_btn.setText(QCoreApplication.translate("MainWindow", u"Start", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"Shader:", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.shader_tab), QCoreApplication.translate("MainWindow", u"Custom", None))
        self.key_1.setText(QCoreApplication.translate("MainWindow", u"set", None))
        self.img_b_1.setText(QCoreApplication.translate("MainWindow", u"Browse", None))
        self.oled_preview_1.setText(QCoreApplication.translate("MainWindow", u"Preview", None))
        self.key_combo_1.setText(QCoreApplication.translate("MainWindow", u"key combination", None))
        self.label_12.setText(QCoreApplication.translate("MainWindow", u"or", None))
        self.key_1_text_set.setText(QCoreApplication.translate("MainWindow", u"set(text)", None))
        self.key_1_c_set.setText(QCoreApplication.translate("MainWindow", u"set(func)", None))
        self.oled_key.setTabText(self.oled_key.indexOf(self.key1_tab), QCoreApplication.translate("MainWindow", u"key 1", None))
        self.key_combo_2.setText(QCoreApplication.translate("MainWindow", u"key combination", None))
        self.key_2_c_set.setText(QCoreApplication.translate("MainWindow", u"set(func)", None))
        self.oled_preview_2.setText(QCoreApplication.translate("MainWindow", u"Preview", None))
        self.key_2_text_set.setText(QCoreApplication.translate("MainWindow", u"set(text)", None))
        self.label_13.setText(QCoreApplication.translate("MainWindow", u"or", None))
        self.img_b_2.setText(QCoreApplication.translate("MainWindow", u"Browse", None))
        self.key_2.setText(QCoreApplication.translate("MainWindow", u"set", None))
        self.oled_key.setTabText(self.oled_key.indexOf(self.key2_tab), QCoreApplication.translate("MainWindow", u"key 2", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.oled_tab), QCoreApplication.translate("MainWindow", u"OLED", None))
        self.menuProgram.setTitle(QCoreApplication.translate("MainWindow", u"Program", None))
    # retranslateUi


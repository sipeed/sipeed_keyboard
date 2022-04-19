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
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QGraphicsView, QGridLayout, QLabel,
    QLineEdit, QMainWindow, QMenuBar, QPlainTextEdit,
    QPushButton, QSizePolicy, QSpinBox, QStatusBar,
    QTabWidget, QTextEdit, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(492, 639)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.gridLayout = QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.logbox = QPlainTextEdit(self.centralwidget)
        self.logbox.setObjectName(u"logbox")
        self.logbox.setMinimumSize(QSize(480, 0))
        self.logbox.setMaximumSize(QSize(16777215, 128))

        self.gridLayout.addWidget(self.logbox, 8, 0, 1, 2)

        self.label_6 = QLabel(self.centralwidget)
        self.label_6.setObjectName(u"label_6")

        self.gridLayout.addWidget(self.label_6, 3, 0, 1, 1)

        self.connect_btn = QPushButton(self.centralwidget)
        self.connect_btn.setObjectName(u"connect_btn")
        self.connect_btn.setMinimumSize(QSize(100, 0))
        self.connect_btn.setMaximumSize(QSize(100, 16777215))

        self.gridLayout.addWidget(self.connect_btn, 4, 1, 1, 1)

        self.preview_box = QGraphicsView(self.centralwidget)
        self.preview_box.setObjectName(u"preview_box")
        self.preview_box.setMinimumSize(QSize(300, 100))

        self.gridLayout.addWidget(self.preview_box, 4, 0, 2, 1)

        self.label_7 = QLabel(self.centralwidget)
        self.label_7.setObjectName(u"label_7")

        self.gridLayout.addWidget(self.label_7, 7, 0, 1, 1)

        self.stop_btn = QPushButton(self.centralwidget)
        self.stop_btn.setObjectName(u"stop_btn")

        self.gridLayout.addWidget(self.stop_btn, 5, 1, 1, 1)

        self.label = QLabel(self.centralwidget)
        self.label.setObjectName(u"label")

        self.gridLayout.addWidget(self.label, 0, 0, 3, 2)

        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.builtin_tab = QWidget()
        self.builtin_tab.setObjectName(u"builtin_tab")
        self.gridLayout_2 = QGridLayout(self.builtin_tab)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.mode_set_btn = QPushButton(self.builtin_tab)
        self.mode_set_btn.setObjectName(u"mode_set_btn")

        self.gridLayout_2.addWidget(self.mode_set_btn, 1, 2, 1, 1)

        self.modebox = QSpinBox(self.builtin_tab)
        self.modebox.setObjectName(u"modebox")

        self.gridLayout_2.addWidget(self.modebox, 1, 1, 1, 1)

        self.label_3 = QLabel(self.builtin_tab)
        self.label_3.setObjectName(u"label_3")

        self.gridLayout_2.addWidget(self.label_3, 1, 0, 1, 1)

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

        self.gridLayout.addWidget(self.tabWidget, 6, 0, 1, 2)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 492, 32))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"Keyboard-CTL", None))
        self.label_6.setText(QCoreApplication.translate("MainWindow", u"Preview:", None))
        self.connect_btn.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.label_7.setText(QCoreApplication.translate("MainWindow", u"Log:", None))
        self.stop_btn.setText(QCoreApplication.translate("MainWindow", u"Stop", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"Keyboard Control Panel", None))
        self.mode_set_btn.setText(QCoreApplication.translate("MainWindow", u"Set", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"Mode:", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.builtin_tab), QCoreApplication.translate("MainWindow", u"Built-in Mode", None))
        self.video_play_btn.setText(QCoreApplication.translate("MainWindow", u"Play", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"Video File Path or Stream link:", None))
        self.browsebtn.setText(QCoreApplication.translate("MainWindow", u"Browse...", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.video_tab), QCoreApplication.translate("MainWindow", u"Video Mode", None))
        self.shader_play_btn.setText(QCoreApplication.translate("MainWindow", u"Start", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"Shader:", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.shader_tab), QCoreApplication.translate("MainWindow", u"Shader Mode", None))
    # retranslateUi


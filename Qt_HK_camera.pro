#-------------------------------------------------
#
# Project created by QtCreator 2021-12-03T10:38:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt_HK_camera
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        widget.cpp \
    loginwindow.cpp \
    offlinedata.cpp \
    qmyvideowidget.cpp

HEADERS += \
        widget.h \
    loginwindow.h \
    offlinedata.h \
    qmyvideowidget.h

FORMS += \
        widget.ui \
    loginwindow.ui \
    offlinedata.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -LD:/Git/GitHub_Desktop/Qt_camera/libs -lHCCore -lHCNetSDK -lPlayCtrl -lGdiPlus
LIBS += -LD:/Git/GitHub_Desktop/Qt_camera/libs/HCNetSDKCom -lHCAlarm -lHCGeneralCfgMgr -lHCPreview

DISTFILES += \
    image/Road.png \
    image/SUV MPV.png \
    image/大客车.png \
    image/货车.png \
    image/两厢轿车.png \
    image/面包车.png \
    image/皮卡车.png \
    image/三厢轿车.png \
    image/小货车.png \
    image/小型轿车.png \
    image/小型客车.png \
    data/fusion.json \
    data/radar.json

RESOURCES += \
    resources.qrc


QT += multimedia
QT += multimediawidgets


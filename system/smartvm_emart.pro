QT -= gui
QT += serialport sql
QT += multimedia

CONFIG += c++17 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += BUILDDATE=\\\"$$system(date '+%Y-%m-%d.%H:%M:%S')\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/common $$PWD//include

QMAKE_RPATHDIR += $$PWD/libs
QMAKE_LFLAGS += -Wl,-rpath,$$PWD/libs


LIBS += -lGL
LIBS += -lv4l2
LIBS += -lrt
LIBS += -lmtdev
LIBS += -lzmq
LIBS += -L$$PWD/libs \
        -leapi \
        -lasushwiomanager \
        -lasusio \
        -lAsusWMI

TEMPLATE = app
TARGET=smartvm_emart
SOURCES += \
    AudioManager.cpp \
    AudioThread.cpp \
    ButtonCtrl.cpp \
    ButtonCtrlManager.cpp \
    DoorCtrl.cpp \
    DoorCtrlManager.cpp \
    GpioCtrl.cpp \
   main.cpp \
   common/Config.cpp \
   common/PacketBuffer.cpp \
   common/QtLogWrapper.cpp \
   common/SysUtil.cpp \
   common/Timer.cpp \
   Instance.cpp \
   SmartVM.cpp \
   Weight_rs232.cpp \
   WeightManager.cpp \
   SensorDAQImpl.cpp \
   CameraManager.cpp \
   V4LGrabber.cpp \
   SignalActorImpl.cpp \
   V4LDevice.cpp \
   ImageWriter.cpp \
   Purchase.cpp \
    HttpsManager.cpp \
    ParamsMonitor.cpp


HEADERS += \
    AudioManager.h \
    AudioThread.h \
    ButtonCtrl.h \
    ButtonCtrlManager.h \
    DoorCtrl.h \
    DoorCtrlManager.h \
    GpioCtrl.h \
   common/Config.h \
   common/CSingleTon.h \
   common/PacketBuffer.h \
   common/QtLogWrapper.h \
   common/SysUtil.h \
   common/Timer.h \
   include/Defines.h \
   include/StlTemplate.h \
   Instance.h \
   SmartVM.h \
   Weight_rs232.h \
   WeightManager.h \
   SensorDAQImpl.h \
   CameraManager.h \
   V4LGrabber.h \
   SignalActorImpl.h \
   V4LDevice.h \
   ImageWriter.h \
   AIVsionDefines.h \
   Purchase.h \
    include/omniedgeapi.h \
    include/stb_image.h \
    include/stb_image_write.h \
    HttpsManager.h \
    AIVsionDefines.h \
    ParamsMonitor.h


DISTFILES +=

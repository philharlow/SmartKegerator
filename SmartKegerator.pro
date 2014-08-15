#-------------------------------------------------
#
# Project created by QtCreator 2013-03-16T01:21:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartKegerator
TEMPLATE = app

CONFIG += qwt

SOURCES += main.cpp\
        windows/mainwindow.cpp \
    windows/pouringwindow.cpp \
    app.cpp \
    data/settings.cpp \
    windows/settingswindow.cpp \
    windows/userswindow.cpp \
    data/dataobject.cpp \
    raspicvcam.cpp \
    raspicamcv/RaspiTexUtil.c \
    raspicamcv/tga.c \
    raspicamcv/RaspiVid.c \
    raspicamcv/RaspiTex.c \
    raspicamcv/RaspiStillYUV.c \
    raspicamcv/RaspiPreview.c \
    raspicamcv/RaspiCLI.c \
    raspicamcv/RaspiCamControl.c \
    raspicamcv/gl_scenes/yuv.c \
    raspicamcv/gl_scenes/teapot.c \
    raspicamcv/gl_scenes/square.c \
    raspicamcv/gl_scenes/sobel.c \
    raspicamcv/gl_scenes/models.c \
    raspicamcv/gl_scenes/mirror.c \
    data/keg.cpp \
    data/pour.cpp \
    data/user.cpp \
    windows/historywindow.cpp \
    managers/flowmetermanager.cpp \
    managers/facialtrainermanager.cpp \
    managers/facialrecognitionmanager.cpp \
    data/beer.cpp \
    data/payment.cpp \
    managers/tempsensormanager.cpp \
    managers/gpiomanager.cpp \
    windows/keyboarddialog.cpp \
    widgets/qwtgraphframe.cpp \
    windows/optionselectdialog.cpp \
    widgets/staticscrollarea.cpp \
    widgets/graph/graphframe.cpp \
    widgets/graph/graph.cpp \
    widgets/beerstatsframe.cpp \
    data/constants.cpp

HEADERS  += windows/mainwindow.h \
    windows/pouringwindow.h \
    app.h \
    data/settings.h \
    data/pour.h \
    windows/settingswindow.h \
    windows/userswindow.h \
    data/dataobject.h \
    raspicvcam.h \
    raspicamcv/tga.h \
    raspicamcv/RaspiTexUtil.h \
    raspicamcv/RaspiTex.h \
    raspicamcv/RaspiPreview.h \
    raspicamcv/RaspiCLI.h \
    raspicamcv/RaspiCamControl.h \
    raspicamcv/gl_scenes/yuv.h \
    raspicamcv/gl_scenes/teapot.h \
    raspicamcv/gl_scenes/square.h \
    raspicamcv/gl_scenes/sobel.h \
    raspicamcv/gl_scenes/models.h \
    raspicamcv/gl_scenes/mirror.h \
    raspicamcv/gl_scenes/cube_texture_and_coords.h \
    data/keg.h \
    data/user.h \
    windows/historywindow.h \
    managers/flowmetermanager.h \
    managers/facialtrainermanager.h \
    managers/facialrecognitionmanager.h \
    data/beer.h \
    shared.h \
    data/payment.h \
    managers/tempsensormanager.h \
    managers/gpiomanager.h \
    windows/keyboarddialog.h \
    widgets/qwtgraphframe.h \
    windows/optionselectdialog.h \
    widgets/staticscrollarea.h \
    rollingqueue.h \
    widgets/graph/graphframe.h \
    widgets/graph/graph.h \
    widgets/beerstatsframe.h \
    data/constants.h

FORMS    += windows/mainwindow.ui \
    windows/pouringwindow.ui \
    windows/settingswindow.ui \
    windows/userswindow.ui \
    windows/historywindow.ui \
    windows/keyboarddialog.ui \
    windows/optionselectdialog.ui

OTHER_FILES += \
    config/users.txt \
    config/pours.txt \
    config/kegs.txt \
    config/config.txt \
    logs/users.txt \
    logs/pours.txt \
    logs/kegs.txt \
    config.txt \
    logs/beers.txt \
    logs/payments.txt \
    qwtStyle.css \
    windows/staticscrollarea.style.css

RESOURCES += \
    resources.qrc

INCLUDEPATH += /usr/local/include \
    /opt/vc/userland/host_applications/linux/libs/bcm_host/include \
    /opt/vc/userland/interface/vcos \
    /opt/vc/userland \
    /opt/vc/userland/interface/vcos/pthreads \
    /opt/vc/userland/interface/vmcs_host/linux \
    /opt/vc/userland/interface/khronos/include \
    /opt/vc/userland/interface/khronos/common \
    /opt/vc/include \
    /opt/vc/include/interface/vcos \
    /opt/vc/include/interface/vcos/pthreads \
    /opt/vc/include/interface/vmcs_host/linux \
    /usr/local/qwt-6.1.0/include

LIBS += -L/usr/local/lib -lwiringPi \
    -L/home/pi/raspberrypi/libfacerec-0.04 -lopencv_facerec \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_objdetect \
    /opt/vc/lib/libmmal_core.so \
    /opt/vc/lib/libmmal_util.so \
    /opt/vc/lib/libmmal_vc_client.so \
    /opt/vc/lib/libvcos.so \
    /opt/vc/lib/libbcm_host.so \
    /opt/vc/lib/libGLESv2.so \
    /opt/vc/lib/libEGL.so \
    -L/usr/local/qwt-6.1.0/lib -lqwt

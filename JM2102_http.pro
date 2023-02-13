#-------------------------------------------------
#
# Project created by QtCreator 2021-08-16T10:03:35
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JM2102_http
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

DEFINES += QT_MESSAGELOGCONTEXT

SOURCES += \
        CJsonObject.cpp \
        bridge.cpp \
        cJSON.c \
        logger.cpp \
        main.cpp \
        mainwindow.cpp \
        my_http.cpp \
        my_match.cpp \
        my_match_thread.cpp \
        my_recv_handle_thread.cpp \
        my_tcpserver.cpp \
        sqllitehandle.cpp \
        zyltools.cpp

HEADERS += \
        CJsonObject.h \
        DataType.h \
        DecodeCardSdk.h \
        Logger.h \
        bridge.h \
        cJSON.h \
        commheader.h \
        mainwindow.h \
        my_http.h \
        my_match.h \
        my_match_thread.h \
        my_recv_handle_thread.h \
        my_tcpserver.h \
        sqllitehandle.h \
        zyltools.h

FORMS += \
        mainwindow.ui
LIBS += -lws2_32
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

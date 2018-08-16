#-------------------------------------------------
#
# Project created by QtCreator 2014-01-29T11:13:52
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
        server.cpp \
    mycamera.cpp

HEADERS  += server.h \
    mycamera.h

FORMS    += server.ui

#-------------------------------------------------
#
# Project created by QtCreator 2013-08-19T22:59:57
#
#-------------------------------------------------

QT       += core gui
QT      += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport


TARGET = my_shag
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    port.cpp

HEADERS  += mainwindow.h \
    port.h

FORMS    += mainwindow.ui

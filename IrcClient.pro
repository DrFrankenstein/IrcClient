#-------------------------------------------------
#
# Project created by QtCreator 2015-04-21T20:08:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IrcClient
TEMPLATE = app
COMPILE += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    ircsession.cpp \
    ircmessage.cpp \
    ircsupportinfo.cpp \
    debugform.cpp \
    servercfgdialog.cpp \
    sessionform.cpp \
    identitydialog.cpp \
    ircuserid.cpp

HEADERS  += mainwindow.h \
    ircsession.h \
    ircmessage.h \
    ircsupportinfo.h \
    debugform.h \
    servercfgdialog.h \
    sessionform.h \
    identitydialog.h \
    ircuserid.h

INCLUDEPATH += C:/Users/Carl/Documents/src/lib/boost_1_58_0

FORMS    += mainwindow.ui \
    debugform.ui \
    servercfgdialog.ui \
    sessionform.ui \
    identitydialog.ui

#QMAKE_CXXFLAGS += -W4
#QMAKE_CXXFLAGS += -Wall
#QMAKE_CXXFLAGS += -analyze

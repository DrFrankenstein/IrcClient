#-------------------------------------------------
#
# Project created by QtCreator 2015-04-21T20:08:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IrcClient
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    ircsession.cpp \
    ircmessage.cpp \
    ircsupportinfo.cpp \
    debugform.cpp \
    servercfgdialog.cpp \
    sessionform.cpp \
    identitydialog.cpp \
    ircuserid.cpp \
    ircchannel.cpp \
    ircnicknamevalidator.cpp \
    ircuser.cpp \
    ircchatbuffer.cpp

HEADERS  += mainwindow.h \
    ircsession.h \
    ircmessage.h \
    ircsupportinfo.h \
    debugform.h \
    servercfgdialog.h \
    sessionform.h \
    identitydialog.h \
    ircuserid.h \
    ircchannel.h \
    ircnicknamevalidator.h \
    ircuser.h \
    ircchatbuffer.h

FORMS    += mainwindow.ui \
    debugform.ui \
    servercfgdialog.ui \
    sessionform.ui \
    identitydialog.ui

RESOURCES += \
    resources.qrc

win32-msvc*:{
    # Uncomment this if you want static analysis warnings. Much slower, but useful when adding new code.
    QMAKE_CXXFLAGS += -analyze

    # I usually compile with a custom makespec with -W4 (instead of the default -W3),
    # but I need certain noisy warnings disabled.
    # C4127: Conditional expression is constant. Triggered by several included templates (QVector among many) that test template parameters.
    QMAKE_CXXFLAGS += -wd4127
    # C4718: recursive call has no side effects, deleting. Triggered by included template code.
    QMAKE_CXXFLAGS += -wd4718
}

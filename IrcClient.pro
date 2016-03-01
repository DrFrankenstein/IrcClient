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
        gui/mainwindow.cpp \
    irc/session.cpp \
    irc/message.cpp \
    irc/supportinfo.cpp \
    gui/debugform.cpp \
    gui/servercfgdialog.cpp \
    gui/sessionform.cpp \
    gui/identitydialog.cpp \
    irc/userid.cpp \
    irc/channel.cpp \
    irc/nicknamevalidator.cpp \
    irc/user.cpp \
    gui/ircchatbuffer.cpp \
    gui/channelform.cpp

HEADERS  += gui/mainwindow.h \
    irc/session.h \
    irc/message.h \
    irc/supportinfo.h \
    gui/debugform.h \
    gui/servercfgdialog.h \
    gui/sessionform.h \
    gui/identitydialog.h \
    irc/userid.h \
    irc/channel.h \
    irc/nicknamevalidator.h \
    irc/user.h \
    gui/ircchatbuffer.h \
    gui/channelform.h

FORMS    += gui/mainwindow.ui \
    gui/debugform.ui \
    gui/servercfgdialog.ui \
    gui/sessionform.ui \
    gui/identitydialog.ui \
    gui/channelform.ui

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

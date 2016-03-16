#include <QObject>
#include <QString>

#include "channel.h"
#include "session.h"
#include "user.h"

namespace Irc
{

Channel::Channel(const QString& name, Session* parent)
    : QObject(parent), _name(name)
{
    this->setObjectName(name);
    
    QObject::connect(this->session(), static_cast<void(Session::*)(QString,QString,QString)>(&Session::privMsgReceived),
                     this,            static_cast<void(Channel::*)(QString,QString,QString)>(&Channel::handleMessage));
}

Session* Channel::session() const
{
    return qobject_cast<Session*>(this->parent());
}

void Channel::part()
{
    this->session()->part(this->_name);
}

void Channel::say(const QString& message)
{
    this->session()->privMsg(this->_name, message);
}

void Channel::handleMessage(QString source, QString target, QString message)
{
    if (target != this->_name)
        return;

    emit messageReceived(source, message);
}

void Channel::handleMessage(User& source, QString target, QString message)
{
    if (target != this->_name)
        return;

    emit messageReceived(source, message);
}

void Channel::handleJoin(User& user, QString channel)
{
    if (channel != this->_name)
        return;

    emit joinReceived(user);
}

void Channel::handlePart(User& user, QString channel, QString message)
{
    if (channel != this->_name)
        return;

    emit partReceived(user, message);
}

void Channel::handleQuit(QString user, QString channel, QString message)
{
    if (channel != this->_name)
        return;
}

}


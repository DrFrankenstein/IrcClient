#include <QObject>
#include <QString>

#include "channel.h"
#include "session.h"
#include "user.h"

namespace Irc
{

Channel::Channel()
{}

Channel::Channel(const QString& name, Session* parent)
    : QObject(parent), _name(name)
{
    this->setObjectName(name);
    
    QObject::connect(this->session(), static_cast<void(Session::*)(QString,QString,QString)>(&Session::privMsgReceived),
                     this,            static_cast<void(Channel::*)(QString,QString,QString)>(&Channel::handleMessage));
    QObject::connect(this->session(), static_cast<void(Session::*)(User&,QString,QString)>(&Session::privMsgReceived),
                     this,            static_cast<void(Channel::*)(User&,QString,QString)>(&Channel::handleMessage));
    QObject::connect(this->session(), static_cast<void(Session::*)(User&,QString)>(&Session::joinReceived),
                     this,            &Channel::handleJoin);
    QObject::connect(this->session(), static_cast<void(Session::*)(User&,QString,QString)>(&Session::partReceived),
                     this,            &Channel::handlePart);
    QObject::connect(this->session(), static_cast<void(Session::*)(QString,QString,QString)>(&Session::topicReceived),
                     this,            static_cast<void(Channel::*)(QString,QString,QString)>(&Channel::handleTopic));
    QObject::connect(this->session(), static_cast<void(Session::*)(User&,QString,QString)>(&Session::topicReceived),
                     this,            static_cast<void(Channel::*)(User&,QString,QString)>(&Channel::handleTopic));
    QObject::connect(this->session(), static_cast<void(Session::*)(QString,QString)>(&Session::quitReceived),
                     this,            &Channel::handleQuit);
}

Session* Channel::session() const
{
    return qobject_cast<Session*>(this->parent());
}

void Channel::part()
{
    if (this->isNull()) return;
    this->session()->part(this->_name);
}

void Channel::say(const QString& message)
{
    if (this->isNull()) return;
    this->session()->privMsg(this->_name, message);
}

bool Channel::isNull() const
{
    return this->_name.isEmpty();
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

void Channel::handleTopic(QString user, QString channel, QString topic)
{
    if (channel != this->_name)
        return;

    this->_topic = topic;

    emit topicChanged(user, topic);
}

void Channel::handleTopic(User& user, QString channel, QString topic)
{
    if (channel != this->_name)
        return;

    emit topicChanged(user, topic);
}

void Channel::handleQuit(QString user, QString message)
{
}

}

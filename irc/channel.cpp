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
    
    connect(this->session(), SIGNAL(messageReceived(QString,QString,QString)),
            this, SLOT(handleMessage(QString,QString,QString)));
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

void Channel::handleMessage(QWeakPointer<User> source, QString target, QString message)
{
    if (target != this->_name)
        return;

    emit messageReceived(source, message);
}

}


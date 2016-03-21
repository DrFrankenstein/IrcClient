#include "user.h"
#include "hostmask.h"

namespace Irc
{

User::User()
{}

User::User(QString hostmask, QObject *parent) : QObject(parent)
{
    
    Hostmask parsed (hostmask);
    this->_nickname = parsed.nickname;
    this->_username = parsed.username;
    this->_host = parsed.host;
}

const QString& User::nickname() const
{
    return this->_nickname;
}

bool User::isNull() const
{
    return this->_nickname.isEmpty();
}

}

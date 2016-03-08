#include "user.h"
#include "hostmask.h"

namespace Irc
{

User::User(QString userid, QObject *parent) : QObject(parent)
{
    
    Hostmask parsed (userid);
    this->_nickname = parsed.nickname;
    this->_username = parsed.username;
    this->_host = parsed.host;
}

}

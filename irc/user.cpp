#include "user.h"
#include "userid.h"

namespace Irc
{

User::User(QString userid, QObject *parent) : QObject(parent)
{
    UserId parsed (userid);
    this->_nickname = parsed.nickname;
    this->_username = parsed.username;
    this->_host = parsed.host;
}

}

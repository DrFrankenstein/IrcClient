#include "ircuser.h"
#include "ircuserid.h"

IrcUser::IrcUser(QString userid, QObject *parent) : QObject(parent)
{
    IrcUserId parsed (userid);
    this->_nickname = parsed.nickname;
    this->_username = parsed.username;
    this->_host = parsed.host;
}

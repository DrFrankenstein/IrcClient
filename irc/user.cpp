#include "user.h"
#include "hostmask.h"
#include "session.h"

#include <memory>

using std::addressof;

namespace Irc
{

User::User()
{}

User::User(QString hostmask, QObject* parent) : QObject(parent)
{
    Hostmask parsed (hostmask);
    this->_nickname = parsed.nickname;
    this->_username = parsed.username;
    this->_host = parsed.host;

    Session* session = qobject_cast<Session*>(parent);
    if (session)
    {
        QObject::connect(session, static_cast<void(Session::*)(User&,QString)>(&Session::nickReceived),
                         this,    &User::handleNickChange);
    }
}

const QString& User::nickname() const
{
    return this->_nickname;
}

bool User::isNull() const
{
    return this->_nickname.isEmpty();
}

void User::handleNickChange(User& user, QString newnick)
{
    if (addressof(user) != this)
        return;

    this->_nickname = newnick;
}

}

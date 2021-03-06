#include "session.h"

#include "message.h"
#include "capabilities.h"
#include "hostmask.h"
#include "channel.h"

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <QMap>
#include <QHash>
#include <utility>

namespace Irc
{

Session::Session(const QString& address, quint16 port,
                       const QString& username, const QStringList& nicknames, const QString& realname,
                       const QString& password, bool invisible, bool wallops, QObject* parent)
    : QObject(parent), _address(address), _port(port), _password(password),
      _username(username), _nicknames(nicknames), _realname(realname),
      _invisible(invisible), _wallops(wallops),
      _state(Offline), _socket(this)
{
    QObject::connect(&this->_socket, &QTcpSocket::readyRead,
                     this,           &Session::socketReadyRead);
    QObject::connect(&this->_socket, &QTcpSocket::stateChanged,
                     this,           &Session::socketStateChanged);
}

Session::~Session()
{
}

const Capabilities& Session::support() const
{
    return this->_capabilities;
}

const QString& Session::networkName() const
{
    const QString& name = this->_capabilities.networkName();

    if (name.isEmpty())
        return this->_address;

    return name;
}

void Session::open()
{
    this->_socket.connectToHost(this->_address, this->_port);
    this->changeState(Connecting);
}

void Session::close()
{
    this->_socket.close();
    this->changeState(Offline);
}

const QString& Session::nickname() const
{
    return this->_currentNickname;
}

void Session::changeNickname(const QString& nick)
{   // [rfc2812 3.1.2]
    if (this->_state == Registering)
        // server doesn't echo our nick change during registration, so we set it eagerly
        this->_currentNickname = nick;

    this->sendMessage("NICK", {nick});
}

bool Session::isMe(const QString &id)
{
    if (Hostmask::isFullyQualified(id))
        return Hostmask(id).nickname == this->_currentNickname;
    else
        return id == this->_currentNickname;
}

bool Session::isMe(const User& user)
{
    return user.nickname() == this->_currentNickname;
}

User& Session::me()
{   // TODO this currently depends on getUser being called with our own hostmask before this
    // we need to find a reliable way to grab it on login.
    return *this->_users.value(this->_currentNickname);
}

void Session::oper(const QString& name, const QString& password)
{   // [rfc2812 3.1.4]
    this->sendMessage("OPER", {name, password});
}

void Session::setMode(const QString& modes)
{   // [rfc2812 3.1.5]
    this->sendMessage("MODE", {modes});
}

void Session::quit(const QString& message)
{   // [rfc2812 3.1.7]
    if (message.isEmpty())
        this->sendMessage(QStringLiteral("QUIT"));
    else
        this->sendMessage("QUIT", {message});
}

void Session::join(const QString& channel, const QString& key)
{   // [rfc2812 3.2.1]
    if (key.isEmpty())
        this->sendMessage("JOIN", { channel });
    else
        this->sendMessage("JOIN", { channel, key });
}

void Session::join(const QStringList& channels, const QStringList& keys)
{   // [rfc2812 3.2.1]
    this->join(channels.join(','), keys.join(','));
}

void Session::part(const QString& channel, const QString& message)
{   // [rfc2812 3.2.2]
    if (message.isEmpty())
        this->sendMessage("PART", { channel });
    else
        this->sendMessage("PART", { channel, message });
}

void Session::part(const QStringList& channels, const QString& message)
{   // [rfc2812 3.2.2]
    this->part(channels.join(','), message);
}

void Session::partAll()
{   // [rfc2812 3.2.1]
    this->join("0");
}

void Session::privMsg(const QString& target, const QString& message)
{   // [rfc2812 3.3.1]
    this->sendMessage("PRIVMSG", { target, message });
}

void Session::sendMessage(const Message& msg)
{
    this->sendRaw(msg.render().toUtf8());   // TODO: support other encodings
}

void Session::sendMessage(const QString& command, const std::initializer_list<QString> params, const QMap<QString, QString>& tags, const QString& prefix)
{
    this->sendMessage(Message(command, params, tags, prefix));
}

void Session::sendMessage(Message::ReplyCode replyCode, const std::initializer_list<QString> params, const QMap<QString, QString>& tags, const QString& prefix)
{
    this->sendMessage(Message(replyCode, params, tags, prefix));
}

void Session::sendRaw(const QByteArray& raw)
{
    this->_socket.write(raw);
    emit rawLineSent(QString(raw));
}

void Session::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    switch (socketState)
    {
    case QAbstractSocket::ConnectedState:
        this->registerUser();
        break;

    default: qt_noop();
    }
}

void Session::socketReadyRead()
{
    while (this->_socket.canReadLine())
    {
        QByteArray raw = this->_socket.readLine();
        emit rawLineReceived(QString(raw));
        Message msg (raw);
        this->handleMessage(msg);
    }
}

void Session::changeState(State state)
{
    this->_state = state;
    emit stateChanged(state);
}

void Session::handleMessage(const Message& msg)
{
    if (msg.isNumeric())
    {
        switch (msg.replyCode())
        {
        case Message::RPL_WELCOME: this->handleRplWelcome(msg); break;
        case Message::RPL_ISUPPORT: this->handleRplISupport(msg); break;
        default: qt_noop();
        }
    }
    else
    {
        typedef void (Session::*Handler)(const Message&);
        static QHash<QString, Handler> handlers {
          {"NICK", &Session::handleNick},
          {"PING", &Session::handlePing},
          {"JOIN", &Session::handleJoin},
          {"PART", &Session::handlePart},
          {"TOPIC", &Session::handleTopic},
          {"PRIVMSG", &Session::handlePrivMsg}
        };

        Handler handler = handlers.value(msg.command());
        if (handler != nullptr)
            (this->*handler)(msg);
    }
}

void Session::handleNick(const Message& msg)
{   // [rfc2812 3.1.2]
    QString user = msg.prefix(),
        newnick = msg.params().at(0);

    if (this->isMe(user))
        this->_currentNickname = newnick;

    emit nickReceived(user, newnick);
    emit nickReceived(this->getUser(user), newnick);
}

void Session::handleQuit(const Message& msg)
{
    // [rfc2812 3.1.7]
    QString user = msg.prefix(),
        reason = msg.params().size() > 0 ? 
            msg.params()[0] : 
            QString();

    emit quitReceived(user, reason);
    emit quitReceived(this->getUser(user), reason);

    this->removeUser(msg.prefix());
}

void Session::handlePing(const Message& msg)
{   // [rfc2812 3.7.2, 3.7.3]
    this->sendMessage("PONG", {msg.params().at(0)});
}

void Session::handleJoin(const Message& msg)
{   // [rfc2812 3.2.1]
    const QString& hostmask = msg.prefix();
    if (msg.params().size() == 0)
        return; // missing channel?

    const QString& channelname = msg.params()[0];

    if (this->isMe(hostmask))
    {
        this->_channels.insert(channelname, new Channel(channelname, this));
    }

    emit joinReceived(hostmask, channelname);
    User& user = this->getUser(hostmask);
    emit joinReceived(user, channelname);
}

void Session::handlePart(const Message & msg)
{   // [rfc2812 3.2.2]
    const QString& hostmask = msg.prefix();
    if (msg.params().size() == 0)
        return; // missing channel?

    const QString& channelname = msg.params()[0],
                 & partMessage = msg.params().size() > 1? msg.params()[1] : QString();
    Channel& ch = this->getChannel(channelname);

    emit partReceived(hostmask, channelname, partMessage);
    User& user = this->getUser(hostmask);
    emit partReceived(user, channelname, partMessage);

    if (this->isMe(hostmask))
        this->_channels.remove(channelname);

    ch.deleteLater();
}

void Session::handleTopic(const Message & msg)
{   // [rfc2812 3.2.4]
    const QString& hostmask = msg.prefix();

    if (msg.params().size() < 2)
        return; // missing channel or topic?

    const QString& channelname = msg.params()[0],
                 & topic = msg.params()[1];

    emit topicReceived(hostmask, channelname, topic);
    User& user = this->getUser(hostmask);
    emit topicReceived(user, channelname, topic);
}

void Session::handlePrivMsg(const Message & msg)
{   // [rfc2812 3.3.1]
    const QString source = msg.prefix();
    if (msg.params().size() < 2)
        return;
    
    auto& params = msg.params();
    const QString& target = params[0],
                 &text = params[1];

    emit privMsgReceived(source, target, text);
    User& user = this->getUser(source);
    emit privMsgReceived(user, target, text);
}

void Session::handleRplWelcome(const Message& msg)
{   // [rfc2812 3.1]
    this->changeState(Online);
    // it would be useful to grab and save our hostmask here, 
    // but few servers actually implement rfc2812 completely. even ircd-seven doesn't do it.
    // rfc1459 does not define RPL_WELCOME at all.
    Q_UNUSED(msg);
}

void Session::handleRplISupport(const Message & msg)
{   // <http://www.irc.org/tech_docs/005.html>
    const QStringList& params = msg.params();
    this->_capabilities.parseISupport(params.constBegin() + 1, params.constEnd()); // + 1 to skip nickname

    emit iSupportReceived(this->_capabilities);
}

void Session::registerUser()
{   // [rfc2812 3.1]
    this->changeState(Registering);

    if (!this->_password.isEmpty())
        this->sendMessage("PASS", {this->_password});

    this->changeNickname(this->_nicknames[0]);

    int modes = 0;
    if (this->_wallops)
        modes |= 4;
    if (this->_invisible)
        modes |= 8;
    this->sendMessage("USER", {this->_username, QString::number(modes), QStringLiteral("*"), this->_realname});
}

User& Session::getUser(const QString& id)
{
    QString nick = Hostmask(id).nickname;
    User* user = this->_users[nick];
    if (!user)
    {
        user = new User(id, this);
        this->_users.insert(user->nickname(), user);
    }

    return *user;
}

void Session::removeUser(const QString& id)
{
    this->_users.remove(Hostmask(id).nickname);
}

Channel& Session::getChannel(const QString & name)
{
    return *this->_channels[name];
}

}

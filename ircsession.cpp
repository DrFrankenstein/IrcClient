#include "ircsession.h"

#include "ircmessage.h"
#include "ircsupportinfo.h"
#include "ircuserid.h"
#include "ircchannel.h"

#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <QMap>
#include <QHash>
#include <QSharedPointer>
#include <utility>

IrcSession::IrcSession(const QString& address, quint16 port,
                       const QString& username, const QStringList& nicknames, const QString& realname,
                       const QString& password, bool invisible, bool wallops, QObject* parent)
    : QObject(parent), _address(address), _port(port), _password(password),
      _username(username), _nicknames(nicknames), _realname(realname),
      _invisible(invisible), _wallops(wallops),
      _state(Offline), _socket(this)
{
    QObject::connect(&this->_socket, SIGNAL(readyRead()),
                     this, SLOT(socketReadyRead()));
    QObject::connect(&this->_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
}

IrcSession::~IrcSession()
{
}

const IrcSupportInfo& IrcSession::support() const
{
    return this->_support;
}

void IrcSession::open()
{
    this->_socket.connectToHost(this->_address, this->_port);
    this->changeState(Connecting);
}

void IrcSession::close()
{
    this->_socket.close();
    this->changeState(Offline);
}

const QString& IrcSession::nickname() const
{
    return this->_currentNickname;
}

void IrcSession::changeNickname(const QString& nick)
{   // [rfc2812 3.1.2]
    if (this->_state == Registering)
        // server doesn't echo our nick change during registration, so we set it eagerly
        this->_currentNickname = nick;

    this->sendMessage("NICK", {nick});
}

bool IrcSession::isMe(const QString &id)
{
    if (IrcUserId::isFullyQualified(id))
        return IrcUserId(id).nickname == this->_currentNickname;
    else
        return id == this->_currentNickname;
}

void IrcSession::oper(const QString& name, const QString& password)
{   // [rfc2812 3.1.4]
    this->sendMessage("OPER", {name, password});
}

void IrcSession::setMode(const QString& modes)
{   // [rfc2812 3.1.5]
    this->sendMessage("MODE", {modes});
}

void IrcSession::quit(const QString& message)
{   // [rfc2812 3.1.7]
    if (message.isEmpty())
        this->sendMessage(QStringLiteral("QUIT"));
    else
        this->sendMessage("QUIT", {message});
}

void IrcSession::join(const QString & channel, const QString & key)
{   // [rfc2812 3.2.1]
    if (key.isEmpty())
        this->sendMessage("JOIN", { channel });
    else
        this->sendMessage("JOIN", { channel, key });
}

void IrcSession::join(const QStringList & channels, const QStringList & keys)
{   // [rfc2812 3.2.1]
    this->join(channels.join(','), keys.join(','));
}

void IrcSession::part(const QString & channel, const QString & message)
{   // [rfc2812 3.2.2]
    if (message.isEmpty())
        this->sendMessage("PART", { channel });
    else
        this->sendMessage("PART", { channel, message });
}

void IrcSession::part(const QStringList & channels, const QString & message)
{   // [rfc2812 3.2.2]
    this->part(channels.join(','), message);
}

void IrcSession::partAll()
{   // [rfc2812 3.2.1]
    this->join("0");
}

void IrcSession::sendMessage(const IrcMessage& msg)
{
    this->sendRaw(msg.render().toUtf8());   // TODO: support other encodings
}

void IrcSession::sendMessage(const QString& command, const std::initializer_list<QString> params, const QMap<QString, QString>& tags, const QString& prefix)
{
    this->sendMessage(IrcMessage(command, params, tags, prefix));
}

void IrcSession::sendMessage(IrcMessage::ReplyCode replyCode, const std::initializer_list<QString> params, const QMap<QString, QString>& tags, const QString& prefix)
{
    this->sendMessage(IrcMessage(replyCode, params, tags, prefix));
}

void IrcSession::sendRaw(const QByteArray& raw)
{
    this->_socket.write(raw);
    emit rawLineSent(QString(raw));
}

void IrcSession::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    switch (socketState)
    {
    case QAbstractSocket::ConnectedState:
        this->registerUser();
        break;
    default: qt_noop();
    }
}

void IrcSession::socketReadyRead()
{
    while (this->_socket.canReadLine())
    {
        QByteArray raw = this->_socket.readLine();
        emit rawLineReceived(QString(raw));
        IrcMessage msg (raw);
        this->handleMessage(msg);
    }
}

void IrcSession::changeState(State state)
{
    this->_state = state;
    emit stateChanged(state);
}

void IrcSession::handleMessage(const IrcMessage& msg)
{
    if (msg.isNumeric())
    {
        switch (msg.replyCode())
        {
        case IrcMessage::RPL_WELCOME: this->handleRplWelcome(msg); break;
        case IrcMessage::RPL_ISUPPORT: this->handleRplISupport(msg); break;
        default: qt_noop();
        }
    }
    else
    {
        typedef void (IrcSession::*Handler)(const IrcMessage&);
        static QHash<QString, Handler> handlers {
          {"NICK", &IrcSession::handleNick},
          {"PING", &IrcSession::handlePing},
          {"JOIN", &IrcSession::handleJoin}
        };

        Handler handler = handlers.value(msg.command());
        if (handler != NULL)
            (this->*handler)(msg);
    }
}

void IrcSession::handleNick(const IrcMessage& msg)
{   // [rfc2812 3.1.2]
    if (this->isMe(msg.prefix()))
        this->_currentNickname = msg.params().at(0);
}

void IrcSession::handlePing(const IrcMessage& msg)
{   // [rfc2812 3.7.2, 3.7.3]
    this->sendMessage("PONG", {msg.params().at(0)});
}

void IrcSession::handleJoin(const IrcMessage& msg)
{   // [rfc2812 3.2.1]
    const QString& userid = msg.prefix();
    if (msg.params().size() == 0)
        return;

    const QString& channelname = msg.params()[0];

    if (this->isMe(userid))
    {
        QSharedPointer<IrcChannel> channel (new IrcChannel(channelname, this));
        this->_channels.insert(channelname, channel);
    }

    emit joinReceived(userid, channelname);
    QSharedPointer<IrcUser> user = this->getUser(userid);
    emit joinReceived(user, channelname);
}

void IrcSession::handleRplWelcome(const IrcMessage& msg)
{   // [rfc2812 3.1]
    this->changeState(Online);
    // it would be useful to grab and save our nick!user@host here, 
    // but few servers actually implement rfc2812 completely. even ircd-seven doesn't do it.
    // rfc1459 does not define RPL_WELCOME at all.
    Q_UNUSED(msg);
}

void IrcSession::handleRplISupport(const IrcMessage & msg)
{   // <http://www.irc.org/tech_docs/005.html>
    const QStringList& params = msg.params();
    this->_support.parseISupport(params.constBegin() + 1, params.constEnd()); // + 1 to skip nickname
}

void IrcSession::registerUser()
{   // [rfc2812 3.1]
    this->changeState(Registering);

    if (!this->_password.isEmpty())
        this->sendMessage("PASS", {this->_password});

    this->changeNickname(this->_nicknames[0]);

    int modes = 0;
    if (this->_wallops) modes |= 4;
    if (this->_invisible) modes |= 8;
    this->sendMessage("USER", {this->_username, QString::number(modes), QStringLiteral("*"), this->_realname});
}

QSharedPointer<IrcUser> IrcSession::getUser(const QString& id)
{
    QString nick = IrcUserId(id).nickname;
    QSharedPointer<IrcUser> user = this->_users[nick];
    if (!user) user.reset(new IrcUser(id, this));

    return user;
}

#include "ircsession.h"

#include "ircmessage.h"
#include "ircsupportinfo.h"
#include "ircuserid.h"

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QMap>
#include <utility>

IrcSession::IrcSession(const QString& address, quint16 port,
                       const QString& username, const QStringList& nicknames, const QString& realname,
                       const QString& password, bool invisible, bool wallops, QObject* parent)
    : QObject(parent), _address(address), _port(port), _password(password),
      _username(username), _nicknames(nicknames), _realname(realname),
      _invisible(invisible), _wallops(wallops),
      _socket(this), _state(Offline)
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
{
    this->sendMessage("MODE", {modes});
}

void IrcSession::quit(const QString& message)
{
    if (message.isEmpty())
        this->sendMessage(QStringLiteral("QUIT"));
    else
        this->sendMessage("QUIT", {message});
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
        }
    }
    else
    {
        typedef void (IrcSession::*Handler)(const IrcMessage&);
        static QMap<QString, Handler> handlers {
          {"NICK", &IrcSession::handleNick},
          {"PING", &IrcSession::handlePing}
        };

        Handler handler = handlers.value(msg.command());
        if (handler != NULL)
            (this->*handler)(msg);
    }
}

void IrcSession::handleNick(const IrcMessage& msg)
{
    if (this->isMe(msg.prefix()))
        this->_currentNickname = msg.params().at(0);
}

void IrcSession::handlePing(const IrcMessage& msg)
{
    this->sendMessage("PONG", {msg.params().at(0)});
}

void IrcSession::handleRplWelcome(const IrcMessage& msg)
{
    this->changeState(Online);
}

void IrcSession::registerUser()
{
    this->changeState(Registering);

    if (!this->_password.isEmpty())
        this->sendMessage("PASS", {this->_password});

    this->changeNickname(this->_nicknames[0]);

    int modes = 0;
    if (this->_wallops) modes |= 4;
    if (this->_invisible) modes |= 8;
    this->sendMessage("USER", {this->_username, QString::number(modes), QStringLiteral("*"), this->_realname});
}

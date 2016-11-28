#ifndef IRC_SESSION_H
#define IRC_SESSION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QHash>
#include <QTcpSocket>
#include <initializer_list>

#include "message.h"
#include "supportinfo.h"
#include "channel.h"
#include "user.h"

class QByteArray;

namespace Irc
{

class Session : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        Offline,
        Connecting,
        Registering,
        Online,
    };

    explicit Session(const QString& address, quint16 port,
                        const QString& username, const QStringList& nicknames, const QString& realname,
                        const QString& password = QString(), bool invisible = true, bool wallops = false,
                        QObject* parent = nullptr);
    ~Session();

    const SupportInfo& support() const;

    const QString& networkName() const;

    void open();
    void close();

    const QString& nickname() const;
    void changeNickname(const QString& nick);
    bool isMe(const QString& id);
    bool isMe(const User& user);

    User& me();

    void oper(const QString& name, const QString& password);

    void setMode(const QString& modes);

    void quit(const QString& message = QString());

    void join(const QString& channel, const QString& key = QString());
    void join(const QStringList& channels, const QStringList& keys = QStringList());

    void part(const QString& channel, const QString& message = QString());
    void part(const QStringList& channels, const QString& message = QString());
    void partAll();

    void privMsg(const QString& target, const QString& message);

    Channel& getChannel(const QString& name);

signals:
    void stateChanged(Session::State state);

    void rawLineReceived(QString line);
    void rawLineSent(QString line);

    void nickReceived(QString sender, QString newnick);
    void nickReceived(User& user, QString newnick);

    void quitReceived(QString sender, QString message);
    void quitReceived(User& user, QString message);

    void joinReceived(QString sender, QString channel);
    void joinReceived(User& user, QString channel);
    void partReceived(QString sender, QString channel, QString message);
    void partReceived(User& user, QString channel, QString message);

    void topicReceived(QString sender, QString channel, QString topic);
    void topicReceived(User& user, QString channel, QString topic);

    void inviteReceived(QString sender, QString channel);
    void inviteReceived(User& user, QString channel);

    void kickReceived(QString sender, QString channel, QString comment);
    void kickReceived(User& user, QString channel, QString comment);

    void privMsgReceived(QString sender, QString target, QString message);
    void privMsgReceived(User& user, QString target, QString message);

    void noticeReceived(QString sender, QString target, QString text);
    void noticeReceived(User& user, QString target, QString text);

    void wallopsReceived(QString sender, QString text);
    void wallopsReceived(User& user, QString text);

    void iSupportReceived(const SupportInfo& info);

public slots:
    void sendMessage(const Message& msg);
    void sendMessage(const QString& command, const std::initializer_list<QString> params,
                     const QMap<QString, QString>& tags = QMap<QString, QString>(),
                     const QString& prefix = QString());
    void sendMessage(Message::ReplyCode replyCode, const std::initializer_list<QString> params,
                     const QMap<QString, QString>& tags = QMap<QString, QString>(),
                     const QString& prefix = QString());
    void sendRaw(const QByteArray& raw);

private slots:
    void socketStateChanged(QAbstractSocket::SocketState socketState);
    void socketReadyRead();

private:
    QString _address;
    quint16 _port;
    QString _password;

    QString _username;
    QList<QString> _nicknames;
    QString _currentNickname;
    QString _realname;

    bool _invisible;
    bool _wallops;

    State _state;

    QTcpSocket _socket;
    SupportInfo _support;

    QHash<QString, Channel*> _channels;
    QHash<QString, User*> _users;

    void changeState(State state);

    void handleMessage(const Message& msg);
    void handleNick(const Message& msg);
    void handleQuit(const Message& msg);
    void handlePing(const Message& msg);

    void handleJoin(const Message& msg);
    void handlePart(const Message& msg);
    void handleTopic(const Message& msg);

    void handlePrivMsg(const Message& msg);

    void handleRplWelcome(const Message& msg);
    void handleRplISupport(const Message& msg);

    void registerUser();
    void pong(const QString& server);

    User& getUser(const QString& id);
    void removeUser(const QString& id);
};

}

#endif // IRC_SESSION_H

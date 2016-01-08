#ifndef IRC_SESSION_H
#define IRC_SESSION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QHash>
#include <QTcpSocket>
#include <QSharedPointer>
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

    void open();
    void close();

    const QString& nickname() const;
    void changeNickname(const QString& nick);
    bool isMe(const QString& id);

    void oper(const QString& name, const QString& password);

    void setMode(const QString& modes);

    void quit(const QString& message = QString());

    void join(const QString& channel, const QString& key = QString());
    void join(const QStringList& channels, const QStringList& keys = QStringList());

    void part(const QString& channel, const QString& message = QString());
    void part(const QStringList& channels, const QString& message = QString());
    void partAll();

signals:
    void stateChanged(Session::State state);

    void rawLineReceived(QString line);
    void rawLineSent(QString line);

    void quitReceived(QString user, QString message);

    void joinReceived(QString user, QString channel);
    void joinReceived(QWeakPointer<User> user, QString channel);
    void partReceived(QString user, QString channel, QString message);
    void partReceived(QWeakPointer<User> user, QString message);

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

    QHash<QString, QSharedPointer<Channel> > _channels;
    QHash<QString, QSharedPointer<User> > _users;

    void changeState(State state);

    void handleMessage(const Message& msg);
    void handleNick(const Message& msg);
    void handlePing(const Message& msg);
    void handleJoin(const Message& msg);

    void handleRplWelcome(const Message& msg);
    void handleRplISupport(const Message& msg);

    void registerUser();
    void pong(const QString& server);

    QSharedPointer<User> getUser(const QString& id);
};

}

#endif // IRC_SESSION_H

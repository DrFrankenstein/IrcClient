#ifndef IRCSESSION_H
#define IRCSESSION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QHash>
#include <QTcpSocket>
#include <QSharedPointer>
#include <initializer_list>

#include "ircmessage.h"
#include "ircsupportinfo.h"
#include "ircchannel.h"
#include "ircuser.h"

class QByteArray;

class IrcSession : public QObject
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

    explicit IrcSession(const QString& address, quint16 port,
                        const QString& username, const QStringList& nicknames, const QString& realname,
                        const QString& password = QString(), bool invisible = true, bool wallops = false,
                        QObject* parent = nullptr);
    ~IrcSession();

    const IrcSupportInfo& support() const;

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
    void stateChanged(IrcSession::State state);

    void rawLineReceived(QString line);
    void rawLineSent(QString line);

    void quitReceived(QString user, QString message);

    void joinReceived(QString user, QString channel);
    void joinReceived(QWeakPointer<IrcUser> user, QString channel);
    void partReceived(QString user, QString channel, QString message);
    void partReceived(QWeakPointer<IrcUser> user, QString message);

public slots:
    void sendMessage(const IrcMessage& msg);
    void sendMessage(const QString& command, const std::initializer_list<QString> params,
                     const QMap<QString, QString>& tags = QMap<QString, QString>(),
                     const QString& prefix = QString());
    void sendMessage(IrcMessage::ReplyCode replyCode, const std::initializer_list<QString> params,
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
    IrcSupportInfo _support;

    QHash<QString, QSharedPointer<IrcChannel> > _channels;
    QHash<QString, QSharedPointer<IrcUser> > _users;

    void changeState(State state);

    void handleMessage(const IrcMessage& msg);
    void handleNick(const IrcMessage& msg);
    void handlePing(const IrcMessage& msg);
    void handleJoin(const IrcMessage& msg);

    void handleRplWelcome(const IrcMessage& msg);
    void handleRplISupport(const IrcMessage& msg);

    void registerUser();
    void pong(const QString& server);

    QSharedPointer<IrcUser> getUser(const QString& id);
};

#endif // IRCSESSION_H

#ifndef IRCSESSION_H
#define IRCSESSION_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QTcpSocket>
#include <initializer_list>

#include "ircmessage.h"
#include "ircsupportinfo.h"

class QByteArray;
class IrcMessage;

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
                        QObject* parent = 0);
    ~IrcSession();

    const IrcSupportInfo& support() const;

    void open();
    void close();

    void changeNickname(const QString& nick);

signals:
    void stateChanged(IrcSession::State state);

    void rawLineReceived(QString line);
    void rawLineSent(QString line);

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
    QString _realname;

    bool _invisible;
    bool _wallops;

    State _state;

    QTcpSocket _socket;
    IrcSupportInfo _support;

    void changeState(State state);

    void handleMessage(const IrcMessage& msg);
    void handlePing(const IrcMessage& msg);

    void registerUser();
    void pong(const QString& server);
};

#endif // IRCSESSION_H

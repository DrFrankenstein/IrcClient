#ifndef IRC_CHANNEL_H
#define IRC_CHANNEL_H

#include <QObject>
#include <QString>

namespace Irc
{

class Session;
class User;

class Channel : public QObject
{
    Q_OBJECT
public:
    Channel();
    explicit Channel(const QString& name, Session* parent = nullptr);

    Session* session() const;

    const QString& name() const;

    void part();
    void say(const QString& message);

    bool isNull() const;

signals:
    void messageReceived(QString source, QString msg);
    void messageReceived(User& source, QString message);
    void joinReceived(QString user);
    void joinReceived(User& user);
    void partReceived(QString user, QString message);
    void partReceived(User& user, QString message);

    void topicChanged(QString user, QString topic);
    void topicChanged(User& user, QString topic);

public slots:

private slots :
    void handleMessage(QString source, QString target, QString message);
    void handleMessage(User& source, QString target, QString message);
    void handleJoin(User& user, QString channel);
    void handlePart(User& user, QString channel, QString message);
    void handleTopic(QString user, QString channel, QString topic);
    void handleTopic(User& user, QString channel, QString topic);
    void handleQuit(QString user, QString message);

private:
    QString _name, _topic;
};

}

#endif // IRC_CHANNEL_H

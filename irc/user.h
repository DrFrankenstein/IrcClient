#ifndef IRC_USER_H
#define IRC_USER_H

#include <QObject>
#include <QString>

namespace Irc
{

class User : public QObject
{
    Q_OBJECT
public:
    User();
    explicit User(QString hostmask, QObject *parent = 0);

    const QString& nickname() const;

    bool isNull() const;

signals:

public slots:
    void handleNickChange(User&, QString newnick);

private:
    QString _nickname, _username, _host;
};

}

#endif // IRC_USER_H

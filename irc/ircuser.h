#ifndef IRCUSER_H
#define IRCUSER_H

#include <QObject>
#include <QString>

namespace Irc
{

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QString userid, QObject *parent = 0);

signals:

public slots:

private:
    QString _nickname, _username, _host;
};

}

#endif // IRCUSER_H

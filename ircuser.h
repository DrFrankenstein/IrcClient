#ifndef IRCUSER_H
#define IRCUSER_H

#include <QObject>
#include <QString>

class IrcUser : public QObject
{
    Q_OBJECT
public:
    explicit IrcUser(QObject *parent = 0);

signals:

public slots:

private:
    QString _nickname, _username, _host;
};

#endif // IRCUSER_H

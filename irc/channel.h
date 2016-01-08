#ifndef IRC_CHANNEL_H
#define IRC_CHANNEL_H

#include <QObject>
#include <QString>

namespace Irc
{

class Channel : public QObject
{
    Q_OBJECT
public:
    explicit Channel(const QString& name, QObject *parent = nullptr);

signals:

public slots:

private:
    QString _name, _topic;
};

}

#endif // IRC_CHANNEL_H

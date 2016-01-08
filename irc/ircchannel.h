#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

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

#endif // IRCCHANNEL_H

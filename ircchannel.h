#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

#include <QObject>

class IrcChannel : public QObject
{
    Q_OBJECT
public:
    explicit IrcChannel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // IRCCHANNEL_H

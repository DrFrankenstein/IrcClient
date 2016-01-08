#ifndef IRCCHANNEL_H
#define IRCCHANNEL_H

#include <QObject>
#include <QString>

class IrcChannel : public QObject
{
    Q_OBJECT
public:
    explicit IrcChannel(const QString& name, QObject *parent = nullptr);

signals:

public slots:

private:
    QString _name, _topic;
};

#endif // IRCCHANNEL_H

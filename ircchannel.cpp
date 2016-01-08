#include <QObject>
#include <QString>

#include "ircchannel.h"

IrcChannel::IrcChannel(const QString& name, QObject *parent) 
    : QObject(parent), _name(name)
{
    this->setObjectName(name);
}


#include <QObject>
#include <QString>

#include "ircchannel.h"

namespace Irc
{

Channel::Channel(const QString& name, QObject *parent) 
    : QObject(parent), _name(name)
{
    this->setObjectName(name);
}

}


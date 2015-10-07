#include "ircsupportinfo.h"

#include <QChar>
#include <QString>
#include <QStringRef>
#include <QStringList>
#include <QVector>

IrcSupportInfo::IrcSupportInfo()
    : chantypes({QChar('#'), QChar('&')}),
      modes(3),
      nicklen(9),
      casemapping(Rfc1459),
      channellen(200)
{
    this->prefix.insert('@', 'o');
    this->prefix.insert('+', 'v');
}

IrcSupportInfo::~IrcSupportInfo()
{

}

void IrcSupportInfo::parseISupport(QStringList params)
{
    for (QString token : params)
    {
        if (token == "are supported by this server")
            return; // end of list

        QVector<QStringRef> parts = token.splitRef('=', QString::SkipEmptyParts);
        QStringRef key = parts[0], value;
        if (parts.size() > 1)
            value = parts[1];


    }
}

QString IrcSupportInfo::unescapeValue(QString val)
{
    for (auto i = val.constBegin(); i != val.constEnd(); i++)
    {

    }

    return "";
}

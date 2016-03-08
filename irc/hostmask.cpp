#include "hostmask.h"

#include <QString>
#include <algorithm>

using std::find;

namespace Irc
{

Hostmask::Hostmask()
{

}

Hostmask::Hostmask(const QString& raw)
{
    this->parse(raw);
}

Hostmask::~Hostmask()
{

}

QString Hostmask::render() const
{
    return QString("%1!%2@%3").arg(this->nickname, this->username, this->host);
}

bool Hostmask::isFullyQualified(const QString& raw)
{   // Just checks for a '!', then a '@' after it.
    auto end = raw.constEnd();
    auto bang = find(raw.constBegin(), end, '!');
    return find(bang, end, '@') != end;
}

bool Hostmask::parse(const QString& raw)
{
    auto it = raw.constBegin(), end = raw.constEnd();

    if (!this->parseNickname(it, end))
        return false;

    if (it == end || *it != '!')
        return false;

    if (!this->parseUsername(it, end))
        return false;

    if (it == end || *it != '@')
        return false;

    return this->parseHost(it, end) && it == end;
}

bool Hostmask::parseNickname(QString::const_iterator& it, const QString::const_iterator& end)
{   // [rfc2812]
    // nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
    QString::const_iterator start = it;

    if (isNickChar(*it, false))
        ++it;
    else
        return false;

    while (it != end && isNickChar(*it, true))
        ++it;

    this->nickname = QString(start, it - start);

    return true;
}

bool Hostmask::parseUsername(QString::const_iterator& it, const QString::const_iterator& end)
{
    QString::const_iterator start = it;

    while (it != end && *it != '@' && *it != '\0' && *it != '\r' && *it != '\n' && *it != ' ')
        ++it;

    this->username = QString(start, it - start);

    return it != start;
}

bool Hostmask::parseHost(QString::const_iterator& it, const QString::const_iterator& end)
{
    QString::const_iterator start = it;

    // We don't validate that the host conforms to RFCs 2812 and 1123 besides from the character set.
    while ((it != end && it->isLetter()) || it->isDigit() || *it == '.' || *it == '-' || *it == ':')
        ++it;

    this->host = QString(start, it - start);

    return it != start;
}

bool Hostmask::isNickChar(QChar ch, bool allowDigits)
{
    return ch.isLetter()
            || ch == '['
            || ch == ']'
            || ch == '\\'
            || ch == '`'
            || ch == '_'
            || ch == '^'
            || ch == '{'
            || ch == '}'
            || ch == '|'
            || (allowDigits && (ch.isNumber() || ch == '-'));
}

}

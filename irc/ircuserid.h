#ifndef IRCUSERID_H
#define IRCUSERID_H

#include <QString>

namespace Irc
{

struct UserId
{
    QString nickname;
    QString username;
    QString host;

public:
    UserId();
    UserId(const QString& raw);
    ~UserId();

    QString render() const;

    static bool isFullyQualified(const QString& raw);

private:
    bool parse(const QString& raw);
    bool parseNickname(QString::const_iterator& it, const QString::const_iterator& end);
    bool parseUsername(QString::const_iterator& it, const QString::const_iterator& end);
    bool parseHost(QString::const_iterator& it, const QString::const_iterator& end);

    static bool isNickChar(QChar ch, bool allowDigits);
};

}

#endif // IRCUSERID_H

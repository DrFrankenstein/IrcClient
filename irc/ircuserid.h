#ifndef IRCUSERID_H
#define IRCUSERID_H

#include <QString>

struct IrcUserId
{
    QString nickname;
    QString username;
    QString host;

public:
    IrcUserId();
    IrcUserId(const QString& raw);
    ~IrcUserId();

    QString render() const;

    static bool isFullyQualified(const QString& raw);

private:
    bool parse(const QString& raw);
    bool parseNickname(QString::const_iterator& it, const QString::const_iterator& end);
    bool parseUsername(QString::const_iterator& it, const QString::const_iterator& end);
    bool parseHost(QString::const_iterator& it, const QString::const_iterator& end);

    static bool isNickChar(QChar ch, bool allowDigits);
};

#endif // IRCUSERID_H

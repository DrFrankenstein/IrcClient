#include <QObject>
#include <QValidator>
#include <QChar>

#include <algorithm>

#include "ircnicknamevalidator.h"

using std::all_of;

IrcNicknameValidator::IrcNicknameValidator(QObject* parent, int nicklen)
    : QValidator(parent), nicklen(nicklen)
{
}

QValidator::State IrcNicknameValidator::validate(QString& input, int& pos) const
{
    if (input.isEmpty()) return QValidator::Intermediate;

    if (nicklen && input.size() > nicklen) return QValidator::Invalid;

    QChar first = input[0];
    if (!(first.isLetter() || isSpecial(first))) return QValidator::Invalid;

    return all_of(input.begin() + 1, input.end(), [](QChar c) {
        return c.isLetter() || c.isDigit() || isSpecial(c) || c == '-';
    })? QValidator::Acceptable : QValidator::Invalid;

    Q_UNUSED(pos);
}

bool IrcNicknameValidator::isSpecial(QChar c)
{
    return c == '['
        || c == ']'
        || c == '\\'
        || c == '`'
        || c == '_'
        || c == '^'
        || c == '{'
        || c == '|'
        || c == '}';
}

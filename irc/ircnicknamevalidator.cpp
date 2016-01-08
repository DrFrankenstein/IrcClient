#include <QObject>
#include <QValidator>
#include <QChar>

#include <algorithm>

#include "ircnicknamevalidator.h"

using std::all_of;

namespace Irc
{

NicknameValidator::NicknameValidator(QObject* parent, int nicklen)
    : QValidator(parent), _nicklen(nicklen)
{
}

QValidator::State NicknameValidator::validate(QString& input, int& pos) const
{
    if (input.isEmpty()) return QValidator::Intermediate;

    if (_nicklen && input.size() > _nicklen) return QValidator::Invalid;

    QChar first = input[0];
    if (!(first.isLetter() || isSpecial(first))) return QValidator::Invalid;

    // 'this' does not need to be captured in this lambda but
    // gcc 4.6-4.7 requires it for static function calls.
    // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51494>
    return all_of(input.begin() + 1, input.end(), [this](QChar c) {
        return c.isLetter() || c.isDigit() || isSpecial(c) || c == '-';
    })? QValidator::Acceptable : QValidator::Invalid;

    Q_UNUSED(pos);
}

bool NicknameValidator::isSpecial(QChar c)
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

}

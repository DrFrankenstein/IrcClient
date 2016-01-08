#ifndef IRCNICKNAMEVALIDATOR_H
#define IRCNICKNAMEVALIDATOR_H

#include <QValidator>
#include <QChar>

namespace Irc
{

class NicknameValidator : public QValidator
{
public:
    NicknameValidator(QObject* parent = nullptr, int nicklen = 0);

    virtual QValidator::State validate(QString& input, int& pos) const;

    static bool isSpecial(QChar c);

private:
    int _nicklen;
};

}

#endif // IRCNICKNAMEVALIDATOR_H

#ifndef IRCNICKNAMEVALIDATOR_H
#define IRCNICKNAMEVALIDATOR_H

#include <QValidator>
#include <QChar>

class IrcNicknameValidator : public QValidator
{
public:
    IrcNicknameValidator(QObject* parent = nullptr, int nicklen = 0);

    virtual QValidator::State validate(QString& input, int& pos) const;

    static bool isSpecial(QChar c);

private:
    int nicklen;
};

#endif // IRCNICKNAMEVALIDATOR_H

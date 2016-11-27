#include "ircchatbuffer.h"

#include "../irc/user.h"

namespace Gui
{

IrcChatBuffer::IrcChatBuffer(QWidget* parent)
    : QTextEdit(parent)
{
    this->setReadOnly(true);
}

void IrcChatBuffer::appendMessage(Irc::User& user, const QString& message)
{
    QTextCursor cur = this->beginNewLine();

    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Bold);
    cur.insertText(QString("<%1> ").arg(user.nickname()), fmt);

    cur.insertText(message);
}

void IrcChatBuffer::appendEvent(Irc::User& user, const QString& message)
{
    QTextCursor cur = this->beginNewLine();
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(Qt::blue));
    cur.insertText("*** ", fmt);

    fmt.setFontWeight(QFont::Bold);
        cur.insertText(user.nickname(), fmt);
    fmt.setFontWeight(QFont::Normal);

    cur.insertText(" ", fmt);
    cur.insertText(message, fmt);
}

QTextCursor IrcChatBuffer::beginNewLine()
{
    QTextCursor cur (this->document());
    cur.movePosition(QTextCursor::End);
    cur.insertBlock();

    return cur;
}

}

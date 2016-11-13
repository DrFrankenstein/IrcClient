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
    QTextCursor cur (this->document());
    cur.movePosition(QTextCursor::End);

    cur.insertBlock();

    QTextCharFormat origfmt = cur.charFormat(),
        fmt;
    fmt.setFontWeight(QFont::Bold);
    cur.mergeCharFormat(fmt);
    cur.insertText(QString("<%1> ").arg(user.nickname()));

    cur.setCharFormat(origfmt);
    cur.insertText(message);
}

}

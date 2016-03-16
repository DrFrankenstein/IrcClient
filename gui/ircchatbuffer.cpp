#include "ircchatbuffer.h"

#include "../irc/user.h"

namespace Gui
{

IrcChatBuffer::IrcChatBuffer(QWidget* parent)
    : QTextEdit(parent)
{
    this->setReadOnly(true);
}

void IrcChatBuffer::appendMessage(QWeakPointer<Irc::User> user, const QString & message)
{
}

}

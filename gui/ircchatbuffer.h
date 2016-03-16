#ifndef GUI_IRCCHATBUFFER_H
#define GUI_IRCCHATBUFFER_H

#include <QObject>
#include <QTextEdit>

#include "../irc/user.h"

namespace Gui
{

class IrcChatBuffer : public QTextEdit
{
    Q_OBJECT

public:
    IrcChatBuffer(QWidget* parent = 0);

private:
    void appendMessage(QWeakPointer<Irc::User> user, const QString& message);
};

}

#endif // GUI_IRCCHATBUFFER_H

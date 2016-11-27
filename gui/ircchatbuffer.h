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

public slots:
    void appendMessage(Irc::User& user, const QString& message);
    void appendEvent(Irc::User& user, const QString& message);

private:
    QTextCursor beginNewLine();
};

}

#endif // GUI_IRCCHATBUFFER_H

#ifndef GUI_IRCCHATBUFFER_H
#define GUI_IRCCHATBUFFER_H

#include <QObject>
#include <QTextEdit>

namespace Gui
{

class IrcChatBuffer : public QTextEdit
{
    Q_OBJECT

public:
    IrcChatBuffer();
};

}

#endif // GUI_IRCCHATBUFFER_H

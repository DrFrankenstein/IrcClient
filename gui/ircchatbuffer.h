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
    IrcChatBuffer(QWidget* parent = 0);
};

}

#endif // GUI_IRCCHATBUFFER_H

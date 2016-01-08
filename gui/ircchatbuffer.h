#ifndef IRCCHATBUFFER_H
#define IRCCHATBUFFER_H

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

#endif // IRCCHATBUFFER_H

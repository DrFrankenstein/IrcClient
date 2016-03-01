#ifndef CHANNELFORM_H
#define CHANNELFORM_H

#include <QWidget>

namespace Irc { class Channel; }

namespace Gui
{

namespace Ui {
class ChannelForm;
}

class ChannelForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelForm(QWidget* parent, Irc::Channel* channel);
    ~ChannelForm();

private:
    Ui::ChannelForm* ui;

    Irc::Channel* _channel;
};

}

#endif // CHANNELFORM_H

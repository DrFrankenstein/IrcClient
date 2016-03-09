#ifndef CHANNELFORM_H
#define CHANNELFORM_H

#include <QWidget>
#include <QSharedPointer>
#include <QWeakPointer>

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
    explicit ChannelForm(QWidget* parent, QWeakPointer<Irc::Channel> channel);
    ~ChannelForm();

private:
    Ui::ChannelForm* ui;

    QSharedPointer<Irc::Channel> _channel;
};

}

#endif // CHANNELFORM_H

#ifndef CHANNELFORM_H
#define CHANNELFORM_H

#include <QWidget>

namespace Irc { class Channel; class User; }

namespace Gui
{

namespace Ui {
class ChannelForm;
}

class ChannelForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelForm(QWidget* parent, Irc::Channel& channel);
    ~ChannelForm();

private slots:
    void handleJoin(Irc::User& user);
    void handlePart(Irc::User& user, QString message);

private:
    Ui::ChannelForm* ui;

    Irc::Channel& _channel;
};

}

#endif // CHANNELFORM_H

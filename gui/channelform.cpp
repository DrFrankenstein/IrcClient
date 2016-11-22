#include "channelform.h"
#include "ui_channelform.h"

#include "../irc/channel.h"

#include <memory>
using std::addressof;

namespace Gui
{

ChannelForm::ChannelForm(QWidget *parent, Irc::Channel& channel) :
    QWidget(parent),
    ui(new Ui::ChannelForm),
    _channel(channel)
{
    ui->setupUi(this);

    QObject::connect(addressof(channel), static_cast<void(Irc::Channel::*)(Irc::User&, QString)>(&Irc::Channel::messageReceived),
                     ui->textEdit,       &IrcChatBuffer::appendMessage);

    this->setWindowTitle(tr("Channel: %1").arg(this->_channel.name()));
}

ChannelForm::~ChannelForm()
{
    delete ui;
}

}

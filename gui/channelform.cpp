#include "channelform.h"
#include "ui_channelform.h"

#include "../irc/channel.h"

namespace Gui
{

ChannelForm::ChannelForm(QWidget *parent, QWeakPointer<Irc::Channel> channel) :
    QWidget(parent),
    ui(new Ui::ChannelForm),
    _channel(channel)
{
    ui->setupUi(this);


}

ChannelForm::~ChannelForm()
{
    delete ui;
}

}

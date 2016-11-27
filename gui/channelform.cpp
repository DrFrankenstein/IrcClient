#include "channelform.h"
#include "ui_channelform.h"

#include "../irc/channel.h"
#include "../irc/user.h"

namespace Gui
{

ChannelForm::ChannelForm(QWidget *parent, Irc::Channel& channel) :
    QWidget(parent),
    ui(new Ui::ChannelForm),
    _channel(channel)
{
    ui->setupUi(this);

    QObject::connect(&channel, static_cast<void(Irc::Channel::*)(Irc::User&, QString)>(&Irc::Channel::messageReceived),
                     ui->textEdit,       &IrcChatBuffer::appendMessage);
    QObject::connect(&channel, static_cast<void(Irc::Channel::*)(Irc::User&)>(&Irc::Channel::joinReceived),
                     this,     &ChannelForm::handleJoin);
    QObject::connect(&channel, static_cast<void(Irc::Channel::*)(Irc::User&,QString)>(&Irc::Channel::partReceived),
                     this,     &ChannelForm::handlePart);

    this->setWindowTitle(tr("Channel: %1").arg(this->_channel.name()));
}

void ChannelForm::handleJoin(Irc::User& user)
{
    ui->textEdit->appendEvent(user, tr("has joined"));
}

void ChannelForm::handlePart(Irc::User& user, QString message)
{
    ui->textEdit->appendEvent(user, tr("has parted (%1)").arg(message));
}

ChannelForm::~ChannelForm()
{
    delete ui;
}

}

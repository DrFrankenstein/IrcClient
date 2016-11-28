#include "channelform.h"
#include "ui_channelform.h"

#include "../irc/channel.h"
#include "../irc/session.h"
#include "../irc/user.h"

#include <QObject>
#include <QWidget>
#include <QLineEdit>

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

ChannelForm::~ChannelForm()
{
    delete ui;
}

void ChannelForm::handleJoin(Irc::User& user)
{
    ui->textEdit->appendEvent(user, tr("has joined"));
}

void ChannelForm::handlePart(Irc::User& user, QString message)
{
    ui->textEdit->appendEvent(user, tr("has parted (%1)").arg(message));
}

void Gui::ChannelForm::on_lineEdit_returnPressed()
{
    QLineEdit* input = ui->lineEdit;
    QString message = input->text();

    ui->textEdit->appendMessage(this->_channel.session()->me(), message);

    this->_channel.say(message);
    input->clear();
}

}

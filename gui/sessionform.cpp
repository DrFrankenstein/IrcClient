#include "sessionform.h"
#include "ui_sessionform.h"

#include "channelform.h"
#include "mainwindow.h"
#include "../irc/channel.h"

#include <QString>
#include <QMdiArea>
#include <QMdiSubWindow>

namespace Gui
{

SessionForm::SessionForm(Irc::Session* session, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SessionForm),
    _session(session)
{
    ui->setupUi(this);
    session->setParent(this);

    QObject::connect(this->_session, &Irc::Session::stateChanged,
                     this, &SessionForm::sessionStateChanged);
    QObject::connect(this->_session, &Irc::Session::iSupportReceived,
                     this,           &SessionForm::onISupportReceived);
    QObject::connect(this->_session, static_cast<void(Irc::Session::*)(Irc::User&,QString)>(&Irc::Session::joinReceived),
                     this,           &SessionForm::onJoin);
    QObject::connect(this->_session, static_cast<void(Irc::Session::*)(Irc::User&,QString)>(&Irc::Session::nickReceived),
                     this,           &SessionForm::onNick);
}

SessionForm::~SessionForm()
{
    delete ui;
}

void SessionForm::sessionStateChanged(Irc::Session::State state)
{
    QString text;

    switch (state)
    {
    case Irc::Session::Offline:
        text = tr("(Offline)");
        break;
    case Irc::Session::Connecting:
        text = tr("Connecting...");
        break;
    case Irc::Session::Registering:
        text = tr("Registering connection...");
        break;
    case Irc::Session::Online:
        text = tr("Connected");
        break;
    }

    ui->networkNameLabel->setText(text);
}

void SessionForm::onISupportReceived(const Irc::SupportInfo& support)
{
    const QString& network = support.networkName();

    if (!network.isEmpty())
        ui->networkNameLabel->setText(network);

    this->updateTitle();
}

void SessionForm::onJoin(Irc::User& user, QString channel)
{
    if (this->_session->isMe(user))
    {
        Irc::Channel& ch = this->_session->getChannel(channel);

        auto form = new ChannelForm(this, ch);
        QMdiArea* mdi = qobject_cast<QMdiSubWindow*>(this->parent())->mdiArea();
        mdi->addSubWindow(form)->show();
    }
}

void SessionForm::onNick(Irc::User &user, QString newnick)
{
    if (this->_session->isMe(newnick))
        this->updateTitle();

    Q_UNUSED(user);
}

void SessionForm::updateTitle()
{
    this->setWindowTitle(tr("Session: %1 on %2").arg(
                             this->_session->nickname(),
                             this->_session->networkName()
                             ));
}

}

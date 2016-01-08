#include "sessionform.h"
#include "ui_sessionform.h"

#include <QString>

namespace Gui
{

SessionForm::SessionForm(Irc::Session* session, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SessionForm),
    _session(session)
{
    ui->setupUi(this);
    session->setParent(this);

    QObject::connect(this->_session, SIGNAL(stateChanged(Session::State)),
                     this, SLOT(sessionStateChanged(Session::State)));
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

}

#include "sessionform.h"
#include "ui_sessionform.h"

#include <QString>

SessionForm::SessionForm(IrcSession* session, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SessionForm),
    _session(session)
{
    ui->setupUi(this);
    session->setParent(this);

    QObject::connect(this->_session, SIGNAL(stateChanged(IrcSession::State)),
                     this, SLOT(sessionStateChanged(IrcSession::State)));
}

SessionForm::~SessionForm()
{
    delete ui;
}

void SessionForm::sessionStateChanged(IrcSession::State state)
{
    QString text;

    switch (state)
    {
    case IrcSession::Offline:
        text = tr("(Offline)");
        break;
    case IrcSession::Connecting:
        text = tr("Connecting...");
        break;
    case IrcSession::Registering:
        text = tr("Registering connection...");
        break;
    case IrcSession::Online:
        text = tr("Connected");
        break;
    }

    ui->networkNameLabel->setText(text);
}

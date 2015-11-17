#ifndef SESSIONFORM_H
#define SESSIONFORM_H

#include <QWidget>

#include "ircsession.h"

namespace Ui {
    class SessionForm;
}

class SessionForm : public QWidget
{
    Q_OBJECT

public:
    explicit SessionForm(IrcSession* session, QWidget* parent = nullptr);
    ~SessionForm();

private slots:
    void sessionStateChanged(IrcSession::State state);

private:
    Ui::SessionForm* ui;

    IrcSession* _session;
};

#endif // SERVERFORM_H

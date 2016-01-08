#ifndef SESSIONFORM_H
#define SESSIONFORM_H

#include <QWidget>

#include "../irc/ircsession.h"

namespace Gui
{

namespace Ui {
    class SessionForm;
}

class SessionForm : public QWidget
{
    Q_OBJECT

public:
    explicit SessionForm(Irc::Session* session, QWidget* parent = nullptr);
    ~SessionForm();

private slots:
    void sessionStateChanged(Irc::Session::State state);

private:
    Ui::SessionForm* ui;

    Irc::Session* _session;
};

}

#endif // SERVERFORM_H

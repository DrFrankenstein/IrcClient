#ifndef GUI_SESSIONFORM_H
#define GUI_SESSIONFORM_H

#include <QWidget>

#include "../irc/session.h"

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
    void onJoin(Irc::User& user, QString channel);

private:
    Ui::SessionForm* ui;

    Irc::Session* _session;
};

}

#endif // GUI_SESSIONFORM_H

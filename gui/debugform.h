#ifndef GUI_DEBUGFORM_H
#define GUI_DEBUGFORM_H

#include <QWidget>

#include "../irc/session.h"

class QBrush;
class QString;

namespace Gui
{

namespace Ui {
class DebugForm;
}

class DebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit DebugForm(Irc::Session* session, QWidget *parent = 0);
    ~DebugForm();

private slots:
    void rawLineReceived(QString line);
    void rawLineSent(QString line);

    void onISupportReceived(const Irc::SupportInfo& support);

    void on_inputLineEdit_returnPressed();

private:
    Ui::DebugForm *ui;

    Irc::Session* _session;

    void addLine(const QBrush& prefixBrush, const QString& prefix, const QString& text);
};

}

#endif // GUI_DEBUGFORM_H

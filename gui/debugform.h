#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include <QWidget>

#include "../irc/ircsession.h"

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

    void on_inputLineEdit_returnPressed();

private:
    Ui::DebugForm *ui;

    Irc::Session* _session;

    void addLine(const QBrush& prefixBrush, const QString& prefix, const QString& text);
};

}

#endif // DEBUGFORM_H

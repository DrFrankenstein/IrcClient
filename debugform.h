#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include <QWidget>

#include "ircsession.h"

class QBrush;
class QString;

namespace Ui {
class DebugForm;
}

class DebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit DebugForm(IrcSession* session, QWidget *parent = 0);
    ~DebugForm();

private slots:
    void rawLineReceived(QString line);
    void rawLineSent(QString line);

    void on_inputLineEdit_returnPressed();

private:
    Ui::DebugForm *ui;

    IrcSession* _session;

    void addLine(const QBrush& prefixBrush, const QString& prefix, const QString& text);
};

#endif // DEBUGFORM_H

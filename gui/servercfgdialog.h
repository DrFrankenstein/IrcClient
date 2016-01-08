#ifndef GUI_SERVERCFGDIALOG_H
#define GUI_SERVERCFGDIALOG_H

#include <QDialog>

namespace Gui
{

namespace Ui {
class ServerCfgDialog;
}

class ServerCfgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerCfgDialog(QWidget *parent = nullptr);
    ~ServerCfgDialog();

    QString address() const;
    quint16 port() const;
    QString password() const;
    bool wallops() const;
    bool invisible() const;

private:
    Ui::ServerCfgDialog *ui;
};

}

#endif // GUI_SERVERCFGDIALOG_H

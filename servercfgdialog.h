#ifndef SERVERCFGDIALOG_H
#define SERVERCFGDIALOG_H

#include <QDialog>

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

#endif // SERVERCFGDIALOG_H

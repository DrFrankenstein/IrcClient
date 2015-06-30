#ifndef IDENTITYDIALOG_H
#define IDENTITYDIALOG_H

#include <QDialog>

namespace Ui {
class IdentityDialog;
}

class IdentityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IdentityDialog(QWidget *parent = 0);
    ~IdentityDialog();

public slots:
    virtual void accept();

private slots:
    void on_addToolButton_clicked();

private:
    Ui::IdentityDialog *ui;

    void load();
};

#endif // IDENTITYDIALOG_H

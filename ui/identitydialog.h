#ifndef IDENTITYDIALOG_H
#define IDENTITYDIALOG_H

#include <QDialog>
#include <QAbstractItemDelegate>

namespace Ui {
class IdentityDialog;
}

class IdentityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IdentityDialog(QWidget *parent = nullptr);
    ~IdentityDialog();

public slots:
    virtual void accept();

private slots:
    void on_addToolButton_clicked();
    void on_deleteToolButton_clicked();
    void on_upToolButton_clicked();
    void on_downToolButton_clicked();

private:
    Ui::IdentityDialog *ui;

    void load();
    void moveCurrentNickname(int diff);
};

#endif // IDENTITYDIALOG_H

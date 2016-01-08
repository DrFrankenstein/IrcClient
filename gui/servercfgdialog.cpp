#include "servercfgdialog.h"
#include "ui_servercfgdialog.h"

namespace Gui
{

ServerCfgDialog::ServerCfgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerCfgDialog)
{
    ui->setupUi(this);
}

ServerCfgDialog::~ServerCfgDialog()
{
    delete ui;
}

QString ServerCfgDialog::address() const
{
    return ui->addressEdit->text();
}

quint16 ServerCfgDialog::port() const
{
    return (quint16) ui->portSpinBox->value();
}

QString ServerCfgDialog::password() const
{
    return ui->passwordEdit->text();
}

bool ServerCfgDialog::invisible() const
{
    return ui->invisibleCheckBox->isChecked();
}

bool ServerCfgDialog::wallops() const
{
    return ui->wallopsCheckBox->isChecked();
}

}

#include "identitydialog.h"
#include "ui_identitydialog.h"

#include <QStringList>
#include <QSettings>

IdentityDialog::IdentityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IdentityDialog)
{
    ui->setupUi(this);
    this->load();
}

IdentityDialog::~IdentityDialog()
{
    delete ui;
}

void IdentityDialog::accept()
{
    QSettings settings;
    settings.beginGroup("identity");
        settings.setValue("username", ui->userNameLineEdit->text());
        settings.setValue("realname", ui->realNameLineEdit->text());

#if 0
        settings.beginWriteArray("nicknames");
        for (int i = 0; i < ui->nicknamesListWidget->count(); i++)
        {
            settings.setArrayIndex(i);
            settings.setValue("nickname", ui->nicknamesListWidget->item(i)->text());
        }
        settings.endArray();
#endif

       QStringList nicks;
       for (int i = 0; i < ui->nicknamesListWidget->count(); i++)
       {
           QString nick = ui->nicknamesListWidget->item(i)->text().trimmed();
           if (!nick.isEmpty()) nicks.append(nick);
       }
       settings.setValue("nicknames", nicks);
    settings.endGroup();

    QDialog::accept();
}

void IdentityDialog::load()
{
    QSettings settings;
    settings.beginGroup("identity");
        ui->userNameLineEdit->setText(settings.value("username").toString());
        ui->realNameLineEdit->setText(settings.value("realname").toString());
        ui->nicknamesListWidget->addItems(settings.value("nicknames").toStringList());
    settings.endGroup();
}

void IdentityDialog::on_addToolButton_clicked()
{
    ui->nicknamesListWidget->addItem("");
}

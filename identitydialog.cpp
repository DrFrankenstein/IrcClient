#include "identitydialog.h"
#include "ui_identitydialog.h"

#include "ircnicknamevalidator.h"

#include <QStringList>
#include <QSettings>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStyledItemDelegate>
#include <QLineEdit>

// Custom item delegate that adds an IrcNicknameValidator to the line edit when editing a nickname entry.
class NicknameItemDelegate : public QStyledItemDelegate
{
public:
    NicknameItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent)
    {
    }

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QWidget* widget = QStyledItemDelegate::createEditor(parent, option, index);

        QLineEdit* editor = qobject_cast<QLineEdit*>(widget);
        if (editor)
            editor->setValidator(new IrcNicknameValidator(editor));

        return widget;
    }
};

IdentityDialog::IdentityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IdentityDialog)
{
    ui->setupUi(this);
    ui->nicknamesListWidget->setItemDelegate(new NicknameItemDelegate(this));

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

       QStringList nicks;
       IrcNicknameValidator validator;
       int count = ui->nicknamesListWidget->count();
       for (int i = 0; i < count; i++)
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

void IdentityDialog::moveCurrentNickname(int diff)
{
    QListWidget* widget = ui->nicknamesListWidget;
    int row = widget->currentRow();
    QListWidgetItem* item = widget->takeItem(row);
    widget->insertItem(row + diff, item);
    widget->setCurrentItem(item);
}

void IdentityDialog::on_addToolButton_clicked()
{
    QListWidget* widget = ui->nicknamesListWidget;
    auto item = new QListWidgetItem(widget);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    widget->setCurrentItem(item);
    widget->editItem(item);
}

void IdentityDialog::on_deleteToolButton_clicked()
{
    for (QListWidgetItem* item : ui->nicknamesListWidget->selectedItems())
        delete item;
}

void IdentityDialog::on_upToolButton_clicked()
{
    this->moveCurrentNickname(-1);
}

void IdentityDialog::on_downToolButton_clicked()
{
    this->moveCurrentNickname(1);
}


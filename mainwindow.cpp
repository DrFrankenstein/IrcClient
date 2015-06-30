#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "servercfgdialog.h"
#include "identitydialog.h"
#include "sessionform.h"
#include "ircsession.h"
#include "debugform.h"

#include <QString>
#include <QSettings>
#include <QStringList>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Connect_to_triggered()
{
    ServerCfgDialog dlg (this);
    if (dlg.exec() == QDialog::Accepted)
        openSession(dlg.address(), dlg.port(), dlg.password(), dlg.invisible(), dlg.wallops());
}

void MainWindow::on_action_Settings_triggered()
{
    IdentityDialog dlg (this);
    dlg.exec();
}

void MainWindow::openSession(const QString& address, quint16 port, const QString& password, bool invisible, bool wallops)
{
    QSettings settings;
    settings.beginGroup("identity");

    IrcSession* session = new IrcSession(address, port,
                                         settings.value("username", "GenericIrcUser").toString(),
                                         settings.value("nicknames", QStringList("Guest1")).toStringList(),
                                         settings.value("realname", "Generic IRC User").toString(),
                                         password, invisible, wallops);
    SessionForm* frm = new SessionForm(session, this);
    ui->mdiArea->addSubWindow(frm)->show();

    DebugForm* deb = new DebugForm(session, this);
    ui->mdiArea->addSubWindow(deb)->show();

    session->open();
}

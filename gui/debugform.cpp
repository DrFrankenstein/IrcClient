#include "debugform.h"
#include "ui_debugform.h"

#include <QTextCursor>
#include <QTextCharFormat>
#include <QBrush>
#include <QFont>
#include <QString>

namespace Gui
{

DebugForm::DebugForm(Irc::Session* session, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugForm),
    _session(session)
{
    ui->setupUi(this);

    QObject::connect(this->_session, &Irc::Session::rawLineReceived,
                     this,           &DebugForm::rawLineReceived);
    QObject::connect(this->_session, &Irc::Session::rawLineSent,
                     this,           &DebugForm::rawLineSent);
}

DebugForm::~DebugForm()
{
    delete ui;
}

void DebugForm::rawLineReceived(QString line)
{
    this->addLine(QBrush(Qt::green), QStringLiteral("--> "), line);
}

void DebugForm::rawLineSent(QString line)
{
    this->addLine(QBrush(Qt::red), QStringLiteral("<-- "), line);
}

void DebugForm::addLine(const QBrush& prefixBrush, const QString& prefix, const QString& text)
{
    QTextCursor cur (ui->outputTextEdit->document());
    cur.movePosition(QTextCursor::End);

    QTextCharFormat origfmt = cur.charFormat();
    QTextCharFormat fmt;
    fmt.setForeground(prefixBrush);
    fmt.setFontWeight(QFont::Bold);
    cur.mergeCharFormat(fmt);
    cur.insertText(prefix);

    cur.setCharFormat(origfmt);
    cur.insertText(text);

    ui->outputTextEdit->setTextCursor(cur);
}

void DebugForm::on_inputLineEdit_returnPressed()
{
    auto edit = ui->inputLineEdit;
    this->_session->sendRaw(edit->text().append("\r\n").toUtf8());
    edit->clear();
}

}

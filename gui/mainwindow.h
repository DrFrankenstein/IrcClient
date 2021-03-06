#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <QMdiSubWindow>

class QWidget;
class QString;

namespace Gui
{

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Connect_to_triggered();

    void on_action_Settings_triggered();

private:
    Ui::MainWindow *ui;

    void openSession(const QString& address, quint16 port, const QString& password, bool invisible, bool wallops);
};

}

#endif // GUI_MAINWINDOW_H

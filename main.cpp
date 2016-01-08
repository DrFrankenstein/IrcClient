#include "gui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gui::MainWindow w;
    w.show();

    a.setOrganizationName("DrFrankenstein");
    a.setApplicationName("Generic IRC Client");

    return a.exec();
}

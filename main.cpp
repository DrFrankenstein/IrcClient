#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    a.setOrganizationName("DrFrankenstein");
    a.setApplicationName("Generic IRC Client");

    return a.exec();
}

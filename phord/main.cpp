#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("kzn.marat");
    QCoreApplication::setOrganizationDomain("kzn.marat.com");
    QCoreApplication::setApplicationName("phord");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

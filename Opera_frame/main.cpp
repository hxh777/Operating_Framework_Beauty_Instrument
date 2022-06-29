#include "mainwindow.h"
#include <QApplication>





int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
    qmlRegisterType<Cpp1>("People", 1, 0, "Person");



    MainWindow w;
    w.show();

    return a.exec();
}

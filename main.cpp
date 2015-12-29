#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();    
    w.autoExec(argv[1],argv[2]);
    return a.exec();
}

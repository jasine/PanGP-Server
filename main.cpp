#include "mainwindow.h"
#include <QApplication>
#include <cstdlib>
#include <cstring>

using std::strcmp;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    
    if(argc>3 && !strcmp(argv[3],"true")){
        w.newGene=true;
    }
    
    //w.show();
    w.autoExec(argv[1],argv[2]);
    return a.exec();
}

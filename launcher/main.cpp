#include <QtGui/QApplication>
#include "mainw.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainW w;
    w.show();
    
    return a.exec();
}

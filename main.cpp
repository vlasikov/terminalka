#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName( "___" );
    QApplication::setOrganizationDomain( "__" );
    QApplication::setApplicationName( "QModBus" );


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}

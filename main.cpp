#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Create the main window on the heap
    MainWindow* w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose); // Ensure window is deleted when closed
    w->show();
    
    return a.exec();
}

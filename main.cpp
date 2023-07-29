
#include "MainWindow.h"

#include <QApplication>
#include <TcpClient.h>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w(new TcpClient("localhost", 2020));
    w.show();
    return a.exec();
}

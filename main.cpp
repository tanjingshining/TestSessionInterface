#include "mainwindow.h"

#include <QApplication>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>
#include <QDBusConnection>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

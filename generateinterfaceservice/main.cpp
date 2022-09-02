#include "mainwindow.h"

#include <QApplication>
#include "QtDBus/QDBusConnection"

#include "test.h"
//#include "test_adaptor.h"
#include "dbusadaptor.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    //qdbusxml2cpp com.qt.example.dbus.xml -a test_adaptor
/*
    Test *test = new Test;
    new DbusAdaptor(test);
    QDBusConnection connect = QDBusConnection::sessionBus();
    connect.registerObject("/", test);
    connect.registerService("com.qt.example.dbus");
*/

    Test *test = new Test();
    DbusAdaptor *adaptor = new DbusAdaptor(test);
    QDBusConnection connect = QDBusConnection::sessionBus();
    connect.registerObject("/", test);
    connect.registerService("com.qt.example.dbus");

    QObject::connect(test, &Test::printStrChanged, adaptor, &DbusAdaptor::strChanged);

    return a.exec();
}

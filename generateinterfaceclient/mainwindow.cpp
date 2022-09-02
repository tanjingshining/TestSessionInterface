#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDBusConnection>
#include <QDebug>
#include <QVariantMap>
#include <QVBoxLayout>
#include <qwidget.h>
//#include "test_interface.h"
#include "qtexampledbus_interface.h"

MyStruct::MyStruct(QString str, bool b)
{
    element1 = str;
    element2 = b;
}

QDBusArgument & operator<<(QDBusArgument &argument, const MyStruct &myStruct)
{
    argument.beginStructure();

    argument << myStruct.element1 << myStruct.element2;

    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MyStruct &myStruct)
{

    argument.beginStructure();

    argument >> myStruct.element1 >> myStruct.element2;

    argument.endStructure();

    return argument;

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    qRegisterMetaType<MyStruct>("MyStruct");

    qDBusRegisterMetaType<MyStruct>();

    method1();

    method2();

    method3();

    getProperty1();

    complexType1();

    complexType2();

    customType1();

    //signal1
    QDBusConnection::sessionBus().connect("com.qt.example.dbus",
                                          "/",
                                          "com.qt.example.dbus",
                                          "strChanged",
                                          this, SLOT(strChanged(QString)));



}


void MainWindow::method1()
{
    //method1
    com::qt::example::dbus *dbus = new com::qt::example::dbus("com.qt.example.dbus", "/", QDBusConnection::sessionBus(), this);
    if(dbus != nullptr){
        //signal2
        connect(dbus, &ComQtExampleDbusInterface::strChanged, [&](QString str){
            qDebug() << "strChanged111" << str;
        });
        qDebug() << dbus->printStr("hello1");
    }

}
void MainWindow::method2()
{
    //method2
    QDBusInterface interface("com.qt.example.dbus",
                             "/",
                             "com.qt.example.dbus",
                             QDBusConnection::sessionBus());
    if (!interface.isValid())
    {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }
    //调用远程的value方法
    interface.call("printStr", QVariant("hello2"));
}
void MainWindow::method3()
{
    //method3
    QDBusMessage message = QDBusMessage::createMethodCall("com.qt.example.dbus",
                                                          "/",
                                                          "com.qt.example.dbus",
                                                          "printStr");
    QList<QVariant> arguments;
    arguments.append(QVariant(QString("hello3")));
    //发送消息
    message.setArguments(arguments);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        qDebug() << "method3 success!";
    }
    else
    {
        qDebug() << "method3 failed!";
    }
}
void MainWindow::complexType1()
{
    //复杂类型解析1
    QStringList list;
    list.append("map1");
    list.append("map2");
    list.append("map3");

    QDBusInterface interface("com.qt.example.dbus",
                             "/",
                             "com.qt.example.dbus",
                             QDBusConnection::sessionBus());
    if (!interface.isValid())
    {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }

    QDBusReply<QVariantMap> reply = interface.call("getMap", QVariant(list));
    if (reply.isValid())
    {
        QVariantMap value = reply.value();
        qDebug() << "value = " << value;
    }
    else
    {
        qDebug() << "value method called failed!";
    }
}

void MainWindow::complexType2()
{
    //复杂类型解析2
    QStringList list2;
    list2.append("map4");
    list2.append("map5");
    list2.append("map6");
    QDBusMessage message2 = QDBusMessage::createMethodCall("com.qt.example.dbus",
                                                           "/",
                                                           "com.qt.example.dbus",
                                                           "getMap");
    QList<QVariant> arguments2;
    arguments2.append(QVariant(list2));
    //发送消息
    message2.setArguments(arguments2);
    QDBusMessage ret = QDBusConnection::sessionBus().call(message2);
    //判断method是否被正确返回
    if (ret.type() != QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        qDebug() << "complex type failed!";
    }
    else
    {
        //参数解析1
        QDBusArgument *darg = static_cast<QDBusArgument*>((void *)ret.arguments()[0].data());
        QVariantMap elems;
        *darg >> elems;
        qDebug() << "complex type elems..." << elems;

        //参数解析2
        QDBusArgument arg = ret.arguments()[0].value<QDBusArgument>();
        QVariantMap map0 = qdbus_cast<QVariantMap>(arg);

        qDebug() << "complex type map..." << map0;
    }
}

void MainWindow::customType1()
{
    //自定义类型1
    QDBusMessage message3 = QDBusMessage::createMethodCall("com.qt.example.dbus",
                                                           "/",
                                                           "com.qt.example.dbus",
                                                           "getStruct");
    QDBusMessage ret3 = QDBusConnection::sessionBus().call(message3);

    if (ret3.type() != QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        qDebug() << "complex type failed!";
    }
    else
    {
        QDBusArgument arg = ret3.arguments()[0].value<QDBusArgument>();
        MyStruct myStruct = qdbus_cast<MyStruct>(arg);

        qDebug() << "myStruct..." << myStruct.element1 <<myStruct.element2;
    }
}

void MainWindow::getProperty1()
{
    QDBusInterface interface("com.qt.example.dbus",
                             "/",
                             "com.qt.example.dbus",
                             QDBusConnection::sessionBus());
    if (!interface.isValid())
    {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        exit(1);
    }
    QString content = interface.property("strContent").toString();
    qDebug() << "getProperty1..." << content;
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::strChanged(QString str)
{
    qDebug() << "strChanged..." << str;
}


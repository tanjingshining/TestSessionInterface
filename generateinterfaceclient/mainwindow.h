#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDBusArgument>
#include <QDBusMetaType>
class MyStruct {

public:
    MyStruct(QString str = "MyStructTestclient", bool b = true);

    friend QDBusArgument &operator<<(QDBusArgument &argument, const MyStruct &myStruct);

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, MyStruct &myStruct);

public:
    QString element1 = "MyStructTestclient";
    bool element2 = false;
};
Q_DECLARE_METATYPE(MyStruct)


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void method1();
    void method2();
    void method3();
    void complexType1();
    void complexType2();
    void customType1();
    void getProperty1();

public slots:
    void strChanged(QString str);
private:
    //Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

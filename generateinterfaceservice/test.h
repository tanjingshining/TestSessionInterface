#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QVariantMap>
#include <QDBusMetaType>
class MyStruct {

public:
    MyStruct(QString str = "MyStructTest", bool b = true);

    friend QDBusArgument &operator<<(QDBusArgument &argument, const MyStruct &myStruct);

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, MyStruct &myStruct);

private:
    QString element1 = "MyStructTest";
    bool element2 = true;
};
Q_DECLARE_METATYPE(MyStruct)

class Test:public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.qt.example.dbus")

    Q_PROPERTY(QString strContent READ strContent WRITE setstrContent NOTIFY strContentChanged)

public:
    Test(QObject* parent=0);

    QString strContent() const;

public Q_SLOTS:
    QString printStr(QString str);
    QVariantMap getMap(QStringList list);
    MyStruct getStruct();

    void setstrContent(QString strContent);

Q_SIGNALS:
    void printStrChanged(QString str);

    void strContentChanged(QString strContent);

private:
    QString m_str = "";
    bool m_inhibited = false;
    QString m_strContent = "testContent";
};

#endif // TEST_H

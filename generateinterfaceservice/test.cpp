#include "test.h"
#include "qdebug.h"

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

Test::Test(QObject* parent)
{
    qRegisterMetaType<MyStruct>("MyStruct");

    qDBusRegisterMetaType<MyStruct>();
}

QString Test::printStr(QString str)
{
    m_str = str;
    qDebug() << "printStr str:" << m_str;
    emit printStrChanged(m_str);
    return m_str;
}

QVariantMap Test::getMap(QStringList list)
{
    QVariantMap map;
    for(int i = 0; i < list.length(); i++)
    {
        QString str = list[i];
        map.insert(QString::number(i), str);
    }
    return map;
}

MyStruct Test::getStruct()
{
    qDebug() << "service getStruct";
    return MyStruct("123", true);
}

QString Test::strContent() const
{
    return m_strContent;
}

void Test::setstrContent(QString strContent)
{
    if (m_strContent == strContent)
        return;

    m_strContent = strContent;
    emit strContentChanged(m_strContent);
}

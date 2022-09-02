/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp com.qt.example.dbus.xml -a test_adaptor
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef TEST_ADAPTOR_H
#define TEST_ADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.qt.example.dbus
 */
class DbusAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.qt.example.dbus")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.qt.example.dbus\">\n"
"    <method name=\"printStr\">\n"
"      <arg direction=\"out\" type=\"s\"/>\n"
"      <arg direction=\"in\" type=\"s\" name=\"str\"/>\n"
"    </method>\n"
"    <method name=\"getMap\">\n"
"      <arg direction=\"out\" type=\"a{sv}\"/>\n"
"      <annotation value=\"QVariantMap\" name=\"org.qtproject.QtDBus.QtTypeName.Out0\"/>\n"
"      <arg direction=\"in\" type=\"av\" name=\"list\"/>\n"
"    </method>\n"
"    <property access=\"read\" type=\"b\" name=\"Inhibited\">\n"
"      <annotation value=\"true\" name=\"com.qt.example.dbus\"/>\n"
"    </property>\n"
"    <signal name=\"strChanged\">\n"
"      <arg direction=\"out\" type=\"s\" name=\"str\"/>\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    DbusAdaptor(QObject *parent);
    virtual ~DbusAdaptor();

public: // PROPERTIES
    Q_PROPERTY(bool Inhibited READ inhibited)
    bool inhibited() const;

public Q_SLOTS: // METHODS
    QVariantMap getMap(const QVariantList &list);
    QString printStr(const QString &str);
Q_SIGNALS: // SIGNALS
    void strChanged(const QString &str);
};

#endif

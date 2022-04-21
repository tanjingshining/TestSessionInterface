#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QDBusMessage>
#include <QCheckBox>
#include <QTextEdit>
#include <QDebug>
#include <QDBusConnection>
#include <QProcess>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>
#include <QFile>
#include <QDir>
#include <sys/file.h>

SystemdInhibit::SystemdInhibit(QObject *parent )
{

}

SystemdInhibit::SystemdInhibit(const SystemdInhibit& systemdInhibit)
{
    m_inhibitFileDescriptor = systemdInhibit.m_inhibitFileDescriptor;
}

SystemdInhibit& SystemdInhibit::operator=(const SystemdInhibit& systemdInhibit)
{
    m_inhibitFileDescriptor = systemdInhibit.m_inhibitFileDescriptor;
    return *this;
}

SystemdInhibit::~SystemdInhibit()
{

}

bool SystemdInhibit::inhibit(QString what, QString who, QString why, QString mode)
{
    QDBusMessage message;

    message = QDBusMessage::createMethodCall("org.freedesktop.login1",

                                             "/org/freedesktop/login1",

                                             "org.freedesktop.login1.Manager",

                                             QStringLiteral("Inhibit"));


    message.setArguments(QVariantList({what, who, why, mode}));

    QDBusPendingReply<QDBusUnixFileDescriptor> reply = QDBusConnection::systemBus().call(message);

    if (!reply.isValid()) {
        qDebug() << "inhibit faile!";
        return false;
    }
    reply.value().swap(m_inhibitFileDescriptor);
    qDebug() << "inhibit success!";
    return true;
}

void SystemdInhibit::uninhibit()
{
    if (!m_inhibitFileDescriptor.isValid()) {
        return;
    }

    m_inhibitFileDescriptor = QDBusUnixFileDescriptor();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->switchUserBtn, &QPushButton::clicked, this, &MainWindow::switchUser);
    connect(ui->suspendBtn, &QPushButton::clicked, this, &MainWindow::suspend);
    connect(ui->hibernateBtn, &QPushButton::clicked, this, &MainWindow::hibernate);
    connect(ui->logoutBtn, &QPushButton::clicked, this, &MainWindow::logout);
    connect(ui->rebootBtn, &QPushButton::clicked, this, &MainWindow::reboot);
    connect(ui->shutdownBtn, &QPushButton::clicked, this, &MainWindow::poweroff);
    connect(ui->sessionInhibitBtn, &QPushButton::clicked, this, &MainWindow::sessionInhibit);
    connect(ui->uninhibitBtn, &QPushButton::clicked, this, &MainWindow::sessionUninhibit);
    connect(ui->systemdInhibitBtn, &QPushButton::clicked, this, &MainWindow::systemdInhibit);

    connect(ui->toolsSwitchUserBtn, &QPushButton::clicked, this, &MainWindow::toolsSwitchUser);
    connect(ui->toolsSuspendBtn, &QPushButton::clicked, this, &MainWindow::toolsSuspend);
    connect(ui->toolsHibernateBtn, &QPushButton::clicked, this, &MainWindow::toolsHibernate);
    connect(ui->toolsLogoutBtn, &QPushButton::clicked, this, &MainWindow::toolsLogout);
    connect(ui->toolsRebootBtn, &QPushButton::clicked, this, &MainWindow::toolsReboot);
    connect(ui->toolsShutdownBtn, &QPushButton::clicked, this, &MainWindow::toolsPowerOff);
    connect(ui->systemUnInhibitBtn, &QPushButton::clicked, this, &MainWindow::systmdUninhibit);

    QStringList modeList;
    modeList.append("block");
    modeList.append("delay");
    ui->modeComBox->addItems(modeList);

    QStringList whatList;
    whatList.append("sleep");
    whatList.append("shutdown");
    whatList.append("idle");
    whatList.append("handle-power-key");
    whatList.append("handle-suspend-key");
    whatList.append("handle-hibernate-key");
    whatList.append("handle-lid-switch");
    ui->whatComboBox->addItems(whatList);
    ui->canCheckBox->setChecked(true);

    ///模拟软件更新的情况 在/tmp/lock/下生成一个带锁的文件kylin-update.lock
    connect(ui->updateCheckBox, &QCheckBox::clicked, [this](bool checked){
        QFile file("/tmp/lock/kylin-update.lock");
        QDir dir("/tmp/lock");
        if(!checked)
        {
            if(file.exists())
                file.remove();
            m_createUpdate = false;
            return;
        }
        else
        {
            if(file.exists())
                file.remove();
            if(!dir.exists("/tmp/lock"))
            {
                //路径不存在，则创建路径
                if(!dir.mkdir("/tmp/lock"))
                {
                    qDebug() << "dir /tmp/lock error!";
                }
            }
            if(!file.open(QIODevice::WriteOnly))
                return;
            m_createUpdate = true;
            file.close();
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
        }
        QProcess p;
        p.start("chmod 000 /tmp/lock/kylin-update.lock");
        p.waitForFinished();
    });

    ///模拟备份的情况 在/tmp/lock/下生成一个带锁的文件kylin-backup.lock
    connect(ui->backupCheckBox, &QCheckBox::clicked, [this](bool checked){
        QFile file("/tmp/lock/kylin-backup.lock");
        QDir dir("/tmp/lock");
        if(!checked)
        {
            if(file.exists())
                file.remove();
            m_createBackUp = false;
            return;
        }
        else
        {
            if(file.exists())
                file.remove();
            if(!dir.exists("/tmp/lock"))
            {
                //路径不存在，则创建路径
                if(!dir.mkdir("/tmp/lock"))
                {
                    qDebug() << "dir /tmp/lock error!";
                }
            }
            if(!file.open(QIODevice::WriteOnly))
                return;
            m_createBackUp = true;
            file.close();
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
        }
        QProcess p;
        p.start("chmod 000 /tmp/lock/kylin-backup.lock");
        p.waitForFinished();
    });

    qDebug() << "currentPath:" << QCoreApplication::applicationDirPath();

    connect(this, &MainWindow::destroyed, [this](QObject *obj ){
        if(m_createBackUp)
        {
            QFile file("/tmp/lock/kylin-backup.lock");
            if(file.exists())
                file.remove();
        }
        if(m_createUpdate)
        {
            QFile file("/tmp/lock/kylin-update.lock");
            if(file.exists())
                file.remove();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::canSwitchUser()
{
    QDBusMessage message0 = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                           "/org/gnome/SessionManager",
                                                           "org.gnome.SessionManager",
                                                           "canSwitch");
    QDBusMessage response0 = QDBusConnection::sessionBus().call(message0);
    if (response0.type() != QDBusMessage::ReplyMessage)
    {
        qDebug() << "canSwitch failed!";
        ui->label_2->setText("canSwitch failed!");
        return false;
    }
    if(!response0.arguments()[0].toBool())
    {
        qDebug() << "canSwitch false!";
        ui->label_2->setText("cann't switchuser");
        return false;
    }
    qDebug() << "canSwitch true!";

    return true;
}

void MainWindow::switchUserDbus()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "switchUser");
    if(ui->argumentCheckBox->isChecked()){
        QList<QVariant> arguments;
        arguments.append(QString(ui->textEdit->toPlainText()));
        message.setArguments(arguments);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "switchUser success!";
        ui->label_2->setText("switchUser success!");
        //exit(0);
    }
    else
    {
        qDebug() << "switchUser failed!";
        ui->label_2->setText("switchUser failed!");

    }
}

void MainWindow::switchUser()
{
    if(ui->canCheckBox->isChecked())
    {
        if(!canSwitchUser())
            return;
    }
    switchUserDbus();
}

bool MainWindow::canSuspend()
{
    QDBusMessage message0 = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                           "/org/gnome/SessionManager",
                                                           "org.gnome.SessionManager",
                                                           "canSuspend");
    QDBusMessage response0 = QDBusConnection::sessionBus().call(message0);
    if (response0.type() != QDBusMessage::ReplyMessage)
    {
        qDebug() << "canSuspend failed!";
        ui->label_2->setText("canSuspend failed!");
        return false;
    }
    if(!response0.arguments()[0].toBool())
    {
        qDebug() << "canSuspend false!";
        ui->label_2->setText("cann't suspend");
        return false;
    }
    qDebug() << "canSuspend true!";
    return true;
}

void MainWindow::suspendDbus()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "suspend");
    if(ui->argumentCheckBox->isChecked()){
        QList<QVariant> arguments;
        arguments.append(QString(ui->textEdit->toPlainText()));
        message.setArguments(arguments);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "suspend success!";
        ui->label_2->setText("suspend success!");
        //exit(0);
    }
    else
    {
        ui->label_2->setText("suspend failed!");
        qDebug() << "suspend failed!";
    }
}

void MainWindow::suspend()
{
    if(ui->canCheckBox->isChecked())
    {
        if(!canSuspend())
            return;
    }
    suspendDbus();
}

bool MainWindow::canHibernate()
{
    QDBusMessage message0 = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                           "/org/gnome/SessionManager",
                                                           "org.gnome.SessionManager",
                                                           "canHibernate");
    QDBusMessage response0 = QDBusConnection::sessionBus().call(message0);
    if (response0.type() != QDBusMessage::ReplyMessage)
    {
        qDebug() << "canHibernate failed!";
        ui->label_2->setText("canHibernate failed!");
        return false;
    }
    if(!response0.arguments()[0].toBool())
    {
        qDebug() << "canHibernate false!";
        ui->label_2->setText("cann't hibernate");
        return false;
    }
    qDebug() << "canHibernate true!";
    return true;
}

void MainWindow::hibernateDbus()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "hibernate");
    if(ui->argumentCheckBox->isChecked()){
        QList<QVariant> arguments;
        arguments.append(QString(ui->textEdit->toPlainText()));
        message.setArguments(arguments);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "hibernate success!";
        ui->label_2->setText("hibernate success!");
        //exit(0);
    }
    else
    {
        qDebug() << "hibernate failed!";
        ui->label_2->setText("hibernate failed!");

    }
}

void MainWindow::hibernate()
{
    if(ui->canCheckBox->isChecked())
    {
        if(!canHibernate())
            return;
    }
    hibernateDbus();
}

bool MainWindow::canLogout()
{
    QDBusMessage message0 = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                           "/org/gnome/SessionManager",
                                                           "org.gnome.SessionManager",
                                                           "canLogout");
    QDBusMessage response0 = QDBusConnection::sessionBus().call(message0);
    if (response0.type() != QDBusMessage::ReplyMessage)
    {
        qDebug() << "canLogout failed!";
        ui->label_2->setText("canLogout failed!");
        return false;
    }
    if(!response0.arguments()[0].toBool())
    {
        qDebug() << "canLogout false!";

        ui->label_2->setText("cann't logout");
        return false;
    }
    qDebug() << "canLogout true!";

    return true;
}

void MainWindow::logoutDbus()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "logout");
    if(ui->argumentCheckBox->isChecked()){
        QList<QVariant> arguments;
        arguments.append(QString(ui->textEdit->toPlainText()));
        message.setArguments(arguments);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "logout success!";
        ui->label_2->setText("logout success!");

        //exit(0);
    }
    else
    {
        qDebug() << "logout failed!";
        ui->label_2->setText("logout failed!");

    }
}

void MainWindow::logout()
{
    if(ui->canCheckBox->isChecked())
    {
        if(!canLogout())
            return;
    }
    logoutDbus();
}

bool MainWindow::canReboot()
{
    QDBusMessage message0 = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                           "/org/gnome/SessionManager",
                                                           "org.gnome.SessionManager",
                                                           "canReboot");
    QDBusMessage response0 = QDBusConnection::sessionBus().call(message0);
    if (response0.type() != QDBusMessage::ReplyMessage)
    {
        qDebug() << "canReboot failed!";
        ui->label_2->setText("canReboot failed!");
        return false;
    }
    if(!response0.arguments()[0].toBool())
    {
        qDebug() << "canReboot false!";

        ui->label_2->setText("cann't reboot");
        return false;
    }
    qDebug() << "canReboot true!";

    return true;
}

void MainWindow::rebootDbus()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "reboot");
    if(ui->argumentCheckBox->isChecked()){
        QList<QVariant> arguments;
        arguments.append(QString(ui->textEdit->toPlainText()));
        message.setArguments(arguments);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "reboot success!";
        ui->label_2->setText("reboot success!");

        //exit(0);
    }
    else
    {
        ui->label_2->setText("reboot failed!");

        qDebug() << "reboot failed!";
    }
}

void MainWindow::reboot()
{
    if(ui->canCheckBox->isChecked())
    {
        if(!canReboot())
            return;
    }
    rebootDbus();
}

bool MainWindow::canPoweroff()
{
    QDBusMessage message0 = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                           "/org/gnome/SessionManager",
                                                           "org.gnome.SessionManager",
                                                           "canPowerOff");
    QDBusMessage response0 = QDBusConnection::sessionBus().call(message0);
    if (response0.type() != QDBusMessage::ReplyMessage)
    {
        qDebug() << "canPowerOff failed!";
        ui->label_2->setText("canPowerOff failed!");
        return false;
    }
    if(!response0.arguments()[0].toBool())
    {
        qDebug() << "canPowerOff false!";

        ui->label_2->setText("cann't poweroff");
        return false;
    }
    qDebug() << "canPowerOff true!";

    return true;
}

void MainWindow::poweroffDbus()
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "powerOff");
    if(ui->argumentCheckBox->isChecked()){
        QList<QVariant> arguments;
        arguments.append(QString(ui->textEdit->toPlainText()));
        message.setArguments(arguments);
    }
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "poweroff success!";
        ui->label_2->setText("poweroff success!");

        //exit(0);
    }
    else
    {
        qDebug() << "poweroff failed!";
        ui->label_2->setText("poweroff failed!");

    }
}

void MainWindow::poweroff()
{
    if(ui->canCheckBox->isChecked())
    {
        if(!canPoweroff())
            return;
    }
    poweroffDbus();
}

///注意flags不能为0
void MainWindow::sessionInhibit()
{
    QString appid = ui->appid->toPlainText();
    QString reason = ui->reason->toPlainText();
    uint toplevel = ui->toplevel->toPlainText().toUInt();
    uint flags = ui->flag->toPlainText().toUInt();

    QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                          "/org/gnome/SessionManager",
                                                          "org.gnome.SessionManager",
                                                          "Inhibit");
    QList<QVariant> arguments;
    arguments.append(appid);
    arguments.append(toplevel);
    arguments.append(reason);
    arguments.append(flags);

    message.setArguments(arguments);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        qDebug() << "Inhibit success!";
        ui->label_16->setText("Inhibit success!");

        inhibitors.append(response.arguments()[0].toUInt());
    }
    else
    {
        qDebug() << "Inhibit failed!";
        ui->label_16->setText("Inhibit failed!");

    }

}

void MainWindow::sessionUninhibit()
{
    for(int i = 0; i < inhibitors.length(); i++)
    {
        QDBusMessage message = QDBusMessage::createMethodCall("org.gnome.SessionManager",
                                                              "/org/gnome/SessionManager",
                                                              "org.gnome.SessionManager",
                                                              "Uninhibit");
        QList<QVariant> arguments;
        arguments.append(inhibitors[i]);
        message.setArguments(arguments);
        QDBusMessage response = QDBusConnection::sessionBus().call(message);
        //判断method是否被正确返回
        if (response.type() == QDBusMessage::ReplyMessage)
        {
            qDebug() << "Uninhibit success!";
            ui->label_16->setText("Uninhibit success!");
        }
        else
        {
            qDebug() << "Uninhibit failed!";
            ui->label_16->setText("Uninhibit failed!");
        }
    }
}

void MainWindow::systemdInhibit()//需要程序一直运行  通过qprocess的方式
{
    QString who = ui->whotext->toPlainText();
    QString why = ui->whytext->toPlainText();

    QStringList arguments;
    arguments.append(ui->whatComboBox->currentText());
    arguments.append(who);
    arguments.append(why);
    arguments.append(ui->modeComBox->currentText());

    SystemdInhibit *sInhibit = new SystemdInhibit();
    if(sInhibit->inhibit(arguments[0], arguments[1], arguments[2], arguments[3]))
    {
        m_systemInhibitList.append(sInhibit);
        ui->label_15->setText("systemd inhibit success!");
        qDebug() << "systemd inhibit success!";
    }
    else
    {
        ui->label_15->setText("systemd inhibit failed!");
        qDebug() << "systemd inhibit failed!";
    }
}

void MainWindow::systmdUninhibit()
{
    for(int i = 0; i < m_systemInhibitList.length(); i++)
    {
        delete(m_systemInhibitList[i]);
    }
}

void MainWindow::toolsSwitchUser()
{
    QProcess::startDetached("ukui-session-tools --switchuser");
}

void MainWindow::toolsHibernate()
{
    QProcess::startDetached("ukui-session-tools --hibernate");
}

void MainWindow::toolsSuspend()
{
    QProcess::startDetached("ukui-session-tools --suspend");
}

void MainWindow::toolsLogout()
{
    QProcess::startDetached("ukui-session-tools --logout");
}

void MainWindow::toolsReboot()
{
    QProcess::startDetached("ukui-session-tools --reboot");
}

void MainWindow::toolsPowerOff()
{
    QProcess::startDetached("ukui-session-tools --shutdown");
}



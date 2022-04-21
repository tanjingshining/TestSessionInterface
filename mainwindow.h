#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDBusArgument>
#include <QDBusMessage>
#include <QProcess>
#include <QDBusPendingReply>
#include <QDBusMessage>
#include <QDBusUnixFileDescriptor>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief 注册Systemd的Inhibit事件及自己主动释放inhibit
 */

class SystemdInhibit : public QObject
{
    Q_OBJECT

public:
    SystemdInhibit(QObject *parent = nullptr);
    SystemdInhibit(const SystemdInhibit&);
    SystemdInhibit& operator=(const SystemdInhibit&);//赋值函数
    ~SystemdInhibit();

    /**
     * @brief Systemd的inhibit
     * @param what 锁类型 shutdown, sleep, idle, handle-power-key, handle-suspend-key, handle-hibernate-key, handle-lid-switch
     * @param who 哪个程序在拿锁
     * @param why  拿锁的原因
     * @param mode 模式 阻塞或延迟 block or delay
     */
    bool inhibit(QString what, QString who, QString why, QString mode);

    /**
     * @brief Systemd的inhibit  根据选则的数据组合，可阻止系统进入休眠、睡眠、重启、关机等
     */
    void uninhibit();

private:
    QDBusUnixFileDescriptor m_inhibitFileDescriptor;

};


/**
 * @brief 调试demo
 * 界面从左到右主要分为五块：systemd的Inhibit接口部分、session的Inhibit接口部分、生成上锁文件、直接调用Session DBUS接口、调用Tools命令
 * 1.systemd的Inhibit接口部分主要包括各种情况下注册Inhibit接口，及一起释放注册了Inhibit接口的资源
 *
 * 2.session的Inhibit接口部分主要是注册session的inhibit接口，阻止进入空闲，播放视频等可以调用，还有恢复空闲uninhibit接口
 *
 * 3.生成上锁文件主要是模拟软件更新、系统备份升级等情况，目前demo只支持生成当前用户下的加锁文件在/tmp/lock目录下   对应的lockfile=true&&lockuser=true，休眠、注销、关机、重启按钮不可用
 *
 * 4.直接调用Session DBUS接口主要是调用session的dbus接口，可以选择需不需要先查询can接口，接口参数是预留的，以后session这些接口可能会支持传入参数，参数为调用该接口的控件名
 *
 * 5.调用Tools命令主要是通过调用ukui-session-tools命令行来实现功能
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    /**
     * @brief Systemd的inhibit  根据选则的数据组合，可阻止系统进入休眠、睡眠、重启、关机等
     */
    void systemdInhibit();
    /**
     * @brief 释放之前阻止系统进入休眠、睡眠、重启、关机等的数据  恢复休眠、睡眠、重启、关机等功能
     */
    void systmdUninhibit();

    ///以下调用的都是session的dbus接口
    /**
     * @brief 查询是否可以切换用户  可在switchUser之前调用
     */
    bool canSwitchUser();

    /**
     * @brief 通过DBUS接口调用switchuser
     */
    void switchUserDbus();

    /**
     * @brief 根据界面的单选框  是否需要选查询canswitchuser、是否带参数来调用switchUserDbus
     */
    void switchUser();


    /**
     * @brief 查询是否可以睡眠  可在suspend之前调用
     */
    bool canSuspend();

    /**
     * @brief 通过DBUS接口调用suspend
     */
    void suspendDbus();

    /**
     * @brief 根据界面的单选框  是否需要选查询canSuspend、是否带参数来调用suspendDbus
     */
    void suspend();


    /**
     * @brief 查询是否可以休眠  可在hibernate之前调用
     */
    bool canHibernate();

    /**
     * @brief 通过DBUS接口调用hibernate
     */
    void hibernateDbus();

    /**
     * @brief 根据界面的单选框  是否需要选查询canHibernate、是否带参数来调用hibernateDbus
     */
    void hibernate();


    /**
     * @brief 查询是否可以注销  可在logout之前调用
     */
    bool canLogout();

    /**
     * @brief 通过DBUS接口调用logout
     */
    void logoutDbus();

    /**
     * @brief 根据界面的单选框  是否需要选查询canLogout、是否带参数来调用logoutDbus
     */
    void logout();


    /**
     * @brief 查询是否可以重启  可在reboot之前调用
     */
    bool canReboot();

    /**
     * @brief 通过DBUS接口调用reboot
     */
    void rebootDbus();

    /**
     * @brief 根据界面的单选框  是否需要选查询canReboot、是否带参数来调用rebootDbus
     */
    void reboot();


    /**
     * @brief 查询是否可以重启  可在poweroff之前调用
     */
    bool canPoweroff();

    /**
     * @brief 通过DBUS接口调用poweroff
     */
    void poweroffDbus();

    /**
     * @brief 根据界面的单选框  是否需要选查询poweroff、是否带参数来调用poweroffDbus
     */
    void poweroff();

    /**
     * @brief 阻止Session进入空闲
     */
    void sessionInhibit();
    /**
     * @brief Session可恢复进入空闲进入空闲
     */
    void sessionUninhibit();


    /**
     * @brief session-manager-tools --switchuser 调用tools命令行
     */
    void toolsSwitchUser();
    /**
     * @brief session-manager-tools --hibernate 调用tools命令行
     */
    void toolsHibernate();
    /**
     * @brief session-manager-tools --suspend 调用tools命令行
     */
    void toolsSuspend();
    /**
     * @brief session-manager-tools --logout 调用tools命令行
     */
    void toolsLogout();
    /**
     * @brief session-manager-tools --reboot 调用tools命令行
     */
    void toolsReboot();
    /**
     * @brief session-manager-tools --poweroff 调用tools命令行
     */
    void toolsPowerOff();

private:
    Ui::MainWindow *ui;
    QList<uint> inhibitors;///注册了session的Inhibit接口的列表
    QList<SystemdInhibit*> m_systemInhibitList;///注册了system接口的对象列表
    bool m_createBackUp = false;
    bool m_createUpdate = false;
};
#endif // MAINWINDOW_H

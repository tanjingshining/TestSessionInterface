### Demo简介

 * 界面从左到右主要分为五块：systemd的Inhibit接口部分、session的Inhibit接口部分、生成上锁文件、直接调用Session DBUS接口、调用Tools命令

 * 1.systemd的Inhibit接口部分主要包括各种情况下注册Inhibit接口，及一起释放注册了Inhibit接口的资源

 * 2.session的Inhibit接口部分主要是注册session的inhibit接口，阻止进入空闲，播放视频等可以调用，还有恢复空闲uninhibit接口

 * 3.生成上锁文件主要是模拟软件更新、系统备份升级等情况，目前demo只支持生成当前用户下的加锁文件在/tmp/lock目录下   对应的lockfile=true&&lockuser=true，休眠、注销、关机、重启按钮不可用

 * 4.直接调用Session DBUS接口主要是调用session的dbus接口，可以选择需不需要先查询can接口，接口参数是预留的，以后session这些接口可能会支持传入参数，参数为调用该接口的控件名

 * 5.调用Tools命令主要是通过调用ukui-session-tools命令行来实现功能

    

 * 各对应功能详情可见代码注释





#### Tools各命令行运行对应情况： 

1. ukui-session-tools --switchuser

​	都可执行；

2. ukui-session-manager --hibernate

​		a.有注册systemd的Inhibit接口的应用，且action为“sleep”，且mode为“block“会给与提示，且提示只有取消按钮；

3. ukui-session-manager --suspend

​		a.同2.a

4. ukui-session-manager --logout

​		a.当lockuser=true且lockfile=true时，给与提示，只有取消按钮；

5. ukui-session-manager --reboot

​		a.有注册systemd的Inhibit接口的应用，且action为“shutdown”，且mode为“block“会给与提示，且提示只有取消按钮；

​		b.在没有注册systemd的Inhibit接口，但有多个用户登录时，提示多个用户登录，可取消可继续；当选择继续时，当lockuser=true或lockfile=true时，给与提示，只有取消按钮；

6.  ukui-session-manager --shutdown

   a.同5.a、5.b两种

   

#### ukui-session-tools界面：

​	按钮不可用情况：

​		1. 查询systemd不支持该操作，按钮隐藏；

​		2. 当lockfile=true&&lockuser=faile，休眠、关机、重启按钮显示但不可用，当lockfile=true&&lockuser=true，休眠、注销、关机、重启按钮显示但不可用；

​	各操作情况：

  1. reboot、shutdown

     a.有注册systemd的Inhibit接口，且action为“shutdown”，且mode为“block“，Tools主界面会给与提示，只有取消按钮；

     b.多用户登录给与提示，可取消可继续；

     c.当lockfile=true || lockuser=true，按钮被禁用

  2. suspend、hibernate

     a.有注册systemd的Inhibit接口的应用，且action为“sleep”，且mode为“block“，Tools主界面会给与提示，只有取消按钮；

     b.当lockfile=true，hibernate按钮被禁用

  3. logout

     a.lockuser=true或lockfile=true时,按钮被禁用；



lockfile什么时候为true？

​	存在上锁文件文件/tmp/lock/kylin-update.lock或/tmp/lock/kylin-update.lock；

lockuser什么时候为true？

​	存在文件/tmp/lock/kylin-backup.lock或文件/tmp/lock/kylin-update.lock,文件上锁且是当前用户创建；



### 问题：

当有应用注册了SessionManager的inhibit接口时

#### 现象1：

无法正常打开电源管理界面，会先弹窗提示“一些程序正在运行，而它不希望你继续操作。”，如果选择放弃，则电源管理界面的重启、关机按钮隐藏；如果选择仍然执行则电源管理界面正常，但再选择重启/关机则又会弹窗提示“一些程序正在运行，而它不希望你继续操作。”此时再选择仍然执行相关重启/关机的操作；放弃则无操作；

#### 现象2：

从电源键选择重启或关机，会先弹窗提示“一些程序正在运行，而它不希望你继续操作。” 会有弹窗提示；

#### 现象3：

用命令 ukui-session-tools --reboot/shutdown，在关机重启需要播放音乐的情况下会先播放音乐，然后再弹窗提示；如果此时命令行关闭，不论选择放弃还是仍然执行都无操作；

#### 原因：

弹窗提示主要来源于dbus提供的canReboot、canPowerOff接口，在该接口中不仅判断systemd是否支持，还判断了SessionManager是否有inhibit的应用；在判断过程中如果有存在注册了inhibit的应用则会弹窗提示“一些程序正在运行，而它不希望你继续操作。”；



1.关机/重启需要判断是否有注册了session的inhibit接口的应用吗？

2.如果问题1需要，那么在can接口里加入提示是否需要调整？如果调整，那在有inhibit的情况下can接口是返回true还是false？




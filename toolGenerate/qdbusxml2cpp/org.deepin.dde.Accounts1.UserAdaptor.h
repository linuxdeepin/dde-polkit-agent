/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp ./dde-polkit-agent/dbus/org.deepin.dde.Accounts1.User.xml -a ./dde-polkit-agent/toolGenerate/qdbusxml2cpp/org.deepin.dde.Accounts1.UserAdaptor -i ./dde-polkit-agent/toolGenerate/qdbusxml2cpp/org.deepin.dde.Accounts1.User.h
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef ORG_DEEPIN_DDE_ACCOUNTS1_USERADAPTOR_H
#define ORG_DEEPIN_DDE_ACCOUNTS1_USERADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include "./dde-polkit-agent/toolGenerate/qdbusxml2cpp/org.deepin.dde.Accounts1.User.h"
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface org.deepin.dde.Accounts1.User
 */
class UserAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.Accounts1.User")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.deepin.dde.Accounts1.User\">\n"
"    <method name=\"DeleteIconFile\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"iconFile\"/>\n"
"    </method>\n"
"    <method name=\"AddGroup\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"group\"/>\n"
"    </method>\n"
"    <method name=\"DeleteGroup\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"group\"/>\n"
"    </method>\n"
"    <method name=\"SetGroups\">\n"
"      <arg direction=\"in\" type=\"as\" name=\"groups\"/>\n"
"    </method>\n"
"    <method name=\"EnableNoPasswdLogin\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"enabled\"/>\n"
"    </method>\n"
"    <method name=\"IsPasswordExpired\">\n"
"      <arg direction=\"out\" type=\"b\"/>\n"
"    </method>\n"
"    <method name=\"PasswordExpiredInfo\">\n"
"      <arg direction=\"out\" type=\"i\" name=\"expiredStatus\"/>\n"
"      <arg direction=\"out\" type=\"i\" name=\"dayLeft\"/>\n"
"    </method>\n"
"    <method name=\"SetAutomaticLogin\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"enabled\"/>\n"
"    </method>\n"
"    <method name=\"SetDesktopBackgrounds\">\n"
"      <arg direction=\"in\" type=\"as\" name=\"backgrounds\"/>\n"
"    </method>\n"
"    <method name=\"SetFullName\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"name\"/>\n"
"    </method>\n"
"    <method name=\"SetGreeterBackground\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"background\"/>\n"
"    </method>\n"
"    <method name=\"SetHistoryLayout\">\n"
"      <arg direction=\"in\" type=\"as\" name=\"layouts\"/>\n"
"    </method>\n"
"    <method name=\"SetHomeDir\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"home\"/>\n"
"    </method>\n"
"    <method name=\"SetIconFile\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"iconFile\"/>\n"
"    </method>\n"
"    <method name=\"SetLayout\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"layout\"/>\n"
"    </method>\n"
"    <method name=\"SetLocale\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"locale\"/>\n"
"    </method>\n"
"    <method name=\"SetLocked\">\n"
"      <arg direction=\"in\" type=\"b\" name=\"locked\"/>\n"
"    </method>\n"
"    <method name=\"SetMaxPasswordAge\">\n"
"      <arg direction=\"in\" type=\"i\" name=\"nDays\"/>\n"
"    </method>\n"
"    <method name=\"SetPassword\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"password\"/>\n"
"    </method>\n"
"    <method name=\"SetPasswordHint\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"hint\"/>\n"
"    </method>\n"
"    <method name=\"SetShell\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"shell\"/>\n"
"    </method>\n"
"    <property access=\"read\" type=\"as\" name=\"Groups\"/>\n"
"    <property access=\"read\" type=\"as\" name=\"IconList\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Shell\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"PasswordStatus\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"SystemAccount\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"Use24HourFormat\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"AccountType\"/>\n"
"    <property access=\"read\" type=\"t\" name=\"LoginTime\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"LongDateFormat\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"LongTimeFormat\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"ShortDateFormat\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"ShortTimeFormat\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"WeekBegins\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"WeekdayFormat\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Locale\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"NoPasswdLogin\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"FullName\"/>\n"
"    <property access=\"read\" type=\"as\" name=\"DesktopBackgrounds\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"GreeterBackground\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"MaxPasswordAge\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"XSession\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"AutomaticLogin\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"IconFile\"/>\n"
"    <property access=\"read\" type=\"b\" name=\"Locked\"/>\n"
"    <property access=\"read\" type=\"as\" name=\"HistoryLayout\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"UserName\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Uid\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Gid\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"HomeDir\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"Layout\"/>\n"
"    <property access=\"read\" type=\"t\" name=\"CreatedTime\"/>\n"
"    <property access=\"read\" type=\"s\" name=\"PasswordHint\"/>\n"
"    <property access=\"read\" type=\"i\" name=\"PasswordLastChange\"/>\n"
"  </interface>\n"
        "")
public:
    UserAdaptor(QObject *parent);
    virtual ~UserAdaptor();

public: // PROPERTIES
    Q_PROPERTY(int AccountType READ accountType)
    int accountType() const;

    Q_PROPERTY(bool AutomaticLogin READ automaticLogin)
    bool automaticLogin() const;

    Q_PROPERTY(qulonglong CreatedTime READ createdTime)
    qulonglong createdTime() const;

    Q_PROPERTY(QStringList DesktopBackgrounds READ desktopBackgrounds)
    QStringList desktopBackgrounds() const;

    Q_PROPERTY(QString FullName READ fullName)
    QString fullName() const;

    Q_PROPERTY(QString Gid READ gid)
    QString gid() const;

    Q_PROPERTY(QString GreeterBackground READ greeterBackground)
    QString greeterBackground() const;

    Q_PROPERTY(QStringList Groups READ groups)
    QStringList groups() const;

    Q_PROPERTY(QStringList HistoryLayout READ historyLayout)
    QStringList historyLayout() const;

    Q_PROPERTY(QString HomeDir READ homeDir)
    QString homeDir() const;

    Q_PROPERTY(QString IconFile READ iconFile)
    QString iconFile() const;

    Q_PROPERTY(QStringList IconList READ iconList)
    QStringList iconList() const;

    Q_PROPERTY(QString Layout READ layout)
    QString layout() const;

    Q_PROPERTY(QString Locale READ locale)
    QString locale() const;

    Q_PROPERTY(bool Locked READ locked)
    bool locked() const;

    Q_PROPERTY(qulonglong LoginTime READ loginTime)
    qulonglong loginTime() const;

    Q_PROPERTY(int LongDateFormat READ longDateFormat)
    int longDateFormat() const;

    Q_PROPERTY(int LongTimeFormat READ longTimeFormat)
    int longTimeFormat() const;

    Q_PROPERTY(int MaxPasswordAge READ maxPasswordAge)
    int maxPasswordAge() const;

    Q_PROPERTY(bool NoPasswdLogin READ noPasswdLogin)
    bool noPasswdLogin() const;

    Q_PROPERTY(QString PasswordHint READ passwordHint)
    QString passwordHint() const;

    Q_PROPERTY(int PasswordLastChange READ passwordLastChange)
    int passwordLastChange() const;

    Q_PROPERTY(QString PasswordStatus READ passwordStatus)
    QString passwordStatus() const;

    Q_PROPERTY(QString Shell READ shell)
    QString shell() const;

    Q_PROPERTY(int ShortDateFormat READ shortDateFormat)
    int shortDateFormat() const;

    Q_PROPERTY(int ShortTimeFormat READ shortTimeFormat)
    int shortTimeFormat() const;

    Q_PROPERTY(bool SystemAccount READ systemAccount)
    bool systemAccount() const;

    Q_PROPERTY(QString Uid READ uid)
    QString uid() const;

    Q_PROPERTY(bool Use24HourFormat READ use24HourFormat)
    bool use24HourFormat() const;

    Q_PROPERTY(QString UserName READ userName)
    QString userName() const;

    Q_PROPERTY(int WeekBegins READ weekBegins)
    int weekBegins() const;

    Q_PROPERTY(int WeekdayFormat READ weekdayFormat)
    int weekdayFormat() const;

    Q_PROPERTY(QString XSession READ xSession)
    QString xSession() const;

public Q_SLOTS: // METHODS
    void AddGroup(const QString &group);
    void DeleteGroup(const QString &group);
    void DeleteIconFile(const QString &iconFile);
    void EnableNoPasswdLogin(bool enabled);
    bool IsPasswordExpired();
    int PasswordExpiredInfo(int &dayLeft);
    void SetAutomaticLogin(bool enabled);
    void SetDesktopBackgrounds(const QStringList &backgrounds);
    void SetFullName(const QString &name);
    void SetGreeterBackground(const QString &background);
    void SetGroups(const QStringList &groups);
    void SetHistoryLayout(const QStringList &layouts);
    void SetHomeDir(const QString &home);
    void SetIconFile(const QString &iconFile);
    void SetLayout(const QString &layout);
    void SetLocale(const QString &locale);
    void SetLocked(bool locked);
    void SetMaxPasswordAge(int nDays);
    void SetPassword(const QString &password);
    void SetPasswordHint(const QString &hint);
    void SetShell(const QString &shell);
Q_SIGNALS: // SIGNALS
};

#endif

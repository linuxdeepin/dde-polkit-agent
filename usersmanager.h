#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>

using AccountsDBus = com::deepin::daemon::Accounts;
using UserDBus = com::deepin::daemon::accounts::User;

class UsersManager : public QObject
{
    Q_OBJECT
public:
    static UsersManager *instance();

    QString getFullName(const QString &name);

private slots:
    void userAdded(const QString &userPath);
    void userRemoved(const QString &userPath);

private:
    QMap<QString, QString> m_fullNameMap;
    QMap<QString, UserDBus*> m_usersMap;
    AccountsDBus *m_accountsInter;

    UsersManager(QObject *parent = nullptr);
};

#endif // USERSMANAGER_H

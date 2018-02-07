/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     Hualet Wang <mr.asianwang@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "usersmanager.h"

static UsersManager *userManagerInstance = Q_NULLPTR;

UsersManager::UsersManager(QObject *parent)
    : QObject(parent),
      m_accountsInter(new AccountsDBus("com.deepin.daemon.Accounts",
                                       "/com/deepin/daemon/Accounts",
                                       QDBusConnection::systemBus(),
                                       this))
{
    const QStringList userList = m_accountsInter->userList();
    for (const QString &userPath : userList) {
        userAdded(userPath);
    }

    connect(m_accountsInter, &AccountsDBus::UserAdded, this, &UsersManager::userAdded);
    connect(m_accountsInter, &AccountsDBus::UserDeleted, this, &UsersManager::userRemoved);
}

UsersManager *UsersManager::instance()
{
    if (!userManagerInstance)
        userManagerInstance = new UsersManager;

    return userManagerInstance;
}

QString UsersManager::getFullName(const QString &name)
{
    return m_fullNameMap.value(name);
}

void UsersManager::userAdded(const QString &userPath)
{
    UserDBus *userInter = new UserDBus("com.deepin.daemon.Accounts",
                                       userPath,
                                       QDBusConnection::systemBus(),
                                       this);
    const QString &username = userInter->userName();

    m_usersMap[userPath] = userInter;
    m_fullNameMap[username] = userInter->fullName();

    connect(userInter, &__User::FullNameChanged, this, [this, username] (const QString &fullname) {
        m_fullNameMap[username]  = fullname;
    });
}

void UsersManager::userRemoved(const QString &userPath)
{
    m_usersMap.remove(userPath);
}

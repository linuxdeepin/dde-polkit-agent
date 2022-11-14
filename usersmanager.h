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

#ifndef USERSMANAGER_H
#define USERSMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>

#include "account_interface.h"
#include "accountuser_interface.h"

using AccountsDBus = org::deepin::dde::Accounts1;
using UserDBus = org::deepin::dde::accounts1::User;

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
    explicit UsersManager(QObject *parent = nullptr);

private:
    QMap<QString, QString> m_fullNameMap;
    QMap<QString, UserDBus*> m_usersMap;
    AccountsDBus *m_accountsInter;
};

#endif // USERSMANAGER_H

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

#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <DDialog>
#include <DPasswordEdit>

#include <QWindow>
#include <QComboBox>

#include <PolkitQt1/Identity>
#include <PolkitQt1/ActionDescription>

namespace PolkitQt1 {
class Details;
}

DWIDGET_USE_NAMESPACE

class QDialogButtonBox;
class QLabel;
class QLineEdit;

class AuthDialog : public DDialog
{
    Q_OBJECT
public:
    enum AuthMode {
        FingerPrint = 0,
        Password = 1
    };

    AuthDialog(const QString &message,
               const QString &iconName);
    ~AuthDialog();

    void setError(const QString &error, bool alertImmediately = false);
    void setRequest(const QString &request, bool requiresAdmin);
    void authenticationFailure(bool &isLock);
    void createUserCB(const PolkitQt1::Identity::List &identities);

    void setAuthInfo(const QString &info);

    void addOptions(QButtonGroup *bg);

    QString password() const;
    void lock();

    PolkitQt1::Identity selectedAdminUser() const;

    bool hasOpenSecurity();
    bool hasSecurityHighLever(QString userName);
signals:
    void adminUserSelected(PolkitQt1::Identity);

private:
    void initUI();
    int getLockLimitTryNum();
    bool passwordIsExpired(PolkitQt1::Identity identity);

private slots:
    void on_userCB_currentIndexChanged(int index);

private:
    QString m_appname;
    QString m_message;
    QString m_iconName;

    QComboBox *m_adminsCombo;
    DPasswordEdit *m_passwordInput;

    int m_numTries;
    int m_lockLimitTryNum;

    void setupUI();
    void showErrorTip();

    QString m_errorMsg;
};
#endif // AUTHDIALOG_H

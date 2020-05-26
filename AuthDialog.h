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

#include <ddialog.h>
#include <dpasswordedit.h>

#include <QWindow>
#include <QComboBox>

#include <PolkitQt1/Identity>
#include <PolkitQt1/ActionDescription>

#include "errortooltip.h"

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

    AuthDialog(const QString &actionId,
               const QString &message,
               const QString &iconName,
               const PolkitQt1::Details &details,
               const PolkitQt1::Identity::List &identities,
               WId parent);
    ~AuthDialog();

    void setError(const QString &error);
    void setRequest(const QString &request, bool requiresAdmin);

    AuthMode authMode();
    void setAuthMode(AuthMode mode);

    void addOptions(QButtonGroup *bg);

    QString password() const;
    void authenticationFailure(int numTries = 0);

    PolkitQt1::Identity adminUserSelected() const;

    PolkitQt1::ActionDescription m_actionDescription;

signals:
    void adminUserSelected(PolkitQt1::Identity);
    void okClicked();
    void usePassword();
    void clearAccessibleMap();

private slots:
    void on_userCB_currentIndexChanged(int index);

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;

private:
    QString m_appname;
    QString m_message;
    QString m_iconName;

    QComboBox *m_adminsCombo;
    DPasswordEdit *m_passwordInput;
    ErrorTooltip *m_tooltip;

    AuthMode m_currentAuthMode;

    void setupUI();
    void createUserCB(const PolkitQt1::Identity::List &identities);
    void showErrorTip();
};

class AuthDetails : public QWidget
{
    Q_OBJECT
public:
    AuthDetails(const PolkitQt1::Details &details,
                const PolkitQt1::ActionDescription &actionDescription,
                const QString &appname,
                QWidget *parent);

private slots:
    void openUrl(const QString &);
    void openAction(const QString &);
};

#endif // AUTHDIALOG_H

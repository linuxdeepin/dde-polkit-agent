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

#ifndef POLICYKITLISTENER_H
#define POLICYKITLISTENER_H

#include <QPointer>
#include <QHash>
#include <QGSettings>

#include <polkit-qt5-1/PolkitQt1/Agent/Listener>

#include <com_deepin_daemon_fprintd.h>
#include <com_deepin_daemon_fprintd_device.h>

#include "libdde-auth/interface/deepinauthinterface.h"

class PluginManager;
class AuthDialog;
class DeepinAuthFramework;

using namespace PolkitQt1::Agent;
using FPrintd = com::deepin::daemon::Fprintd;
using FPrintdDevice = com::deepin::daemon::fprintd::Device;

class PolicyKitListener : public Listener, public DeepinAuthInterface
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.Polkit1AuthAgent")
public:
    PolicyKitListener(QObject *parent = 0);
    virtual ~PolicyKitListener();

public slots:
    void initiateAuthentication(const QString &actionId,
                                const QString &message,
                                const QString &iconName,
                                const PolkitQt1::Details &details,
                                const QString &cookie,
                                const PolkitQt1::Identity::List &identities,
                                PolkitQt1::Agent::AsyncResult *result) override;
    bool initiateAuthenticationFinish() override;
    void cancelAuthentication() override;

    void tryAgain();
    void finishObtainPrivilege();

    void request(const QString &request, bool echo);
    void completed(bool gainedAuthorization);
    void showError(const QString &text);

    void setWIdForAction(const QString &action, qulonglong wID);

    void onDisplayErrorMsg(const QString &errtype, const QString &msg) override;
    void onDisplayTextInfo(const QString &msg) override;
    void onPasswordResult(const QString &msg) override;

private:
    QPointer<AuthDialog> m_dialog;
    QPointer<PluginManager> m_pluginManager;
    QPointer<Session> m_session;
    QPointer<FPrintd> m_fprintdInter;
    QPointer<FPrintdDevice> m_fprintdDeviceInter;
    DeepinAuthFramework *m_deepinAuthFramework;
    QGSettings *m_gsettings = nullptr;

    PolkitQt1::Identity::List m_identities;
    PolkitQt1::Agent::AsyncResult *m_result;
    QString m_cookie;
    QString m_password;
    PolkitQt1::Identity m_selectedUser;
    QHash< QString, qulonglong > m_actionsToWID;
    QTimer m_delayRemoveTimer;

    bool m_inProgress;
    bool m_gainedAuthorization;
    bool m_wasCancelled;
    bool m_usePassword;
    int m_numTries;
    int m_numFPrint;

private slots:
    bool isDeepin();
    void dialogAccepted();
    void dialogCanceled();
    void userSelected(const PolkitQt1::Identity &identity);
    void fprintdDeviceChanged();
};

#endif

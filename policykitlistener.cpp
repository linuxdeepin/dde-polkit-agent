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

#include <QDBusConnection>
#include <QDebug>
#include <QtConcurrent>

#include <polkit-qt5-1/PolkitQt1/Agent/Listener>
#include <polkit-qt5-1/PolkitQt1/Agent/Session>
#include <polkit-qt5-1/PolkitQt1/Subject>
#include <polkit-qt5-1/PolkitQt1/Identity>
#include <polkit-qt5-1/PolkitQt1/Details>
#include <libintl.h>

#include "policykitlistener.h"
#include "AuthDialog.h"

#include "polkit1authagent_adaptor.h"

#include "pluginmanager.h"

#define USE_DEEPIN_AUTH "useDeepinAuth"

PolicyKitListener::PolicyKitListener(QObject *parent)
    : Listener(parent)
    , m_selectedUser(nullptr)
    , m_inProgress(false)
    , m_usePassword(false)
    , m_numFPrint(0)
    , m_showInfoSuccess(false)
{
    (void) new Polkit1AuthAgentAdaptor(this);

    if (QGSettings::isSchemaInstalled("com.deepin.dde.auth.control")) {
        m_gsettings = new QGSettings("com.deepin.dde.auth.control", "/com/deepin/dde/auth/control/", this);
    }

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService("com.deepin.Polkit1AuthAgent")) {
        qWarning() << "Register auth agent service failed!";
    }
    if (!sessionBus.registerObject("/com/deepin/Polkit1AuthAgent", this,
                                   QDBusConnection::ExportScriptableSlots |
                                   QDBusConnection::ExportScriptableProperties |
                                   QDBusConnection::ExportAdaptors)) {
        qWarning() << "Register auth agent path failed!";
    }

    qDebug() << "Listener online";

    m_pluginManager = new PluginManager(this);

    m_delayRemoveTimer.setInterval(3000);
    m_delayRemoveTimer.setSingleShot(true);
    connect(&m_delayRemoveTimer, &QTimer::timeout, this, [ = ] {
        m_dialog.data()->hide();
        // FIXME(hualet): don't know why deleteLater doesn't do its job,
        // combined invokeMethod with Qt::QueuedConnection works well.
        // m_dialog.data()->deleteLater();
        QMetaObject::invokeMethod(m_dialog.data(), "deleteLater", Qt::QueuedConnection);
    });
}

PolicyKitListener::~PolicyKitListener()
{
}

void PolicyKitListener::setWIdForAction(const QString &action, qulonglong wID)
{
    qDebug() << "On to the handshake";
    m_actionsToWID[action] = wID;
}

void PolicyKitListener::initiateAuthentication(const QString &actionId,
                                               const QString &message,
                                               const QString &iconName,
                                               const PolkitQt1::Details &details,
                                               const QString &cookie,
                                               const PolkitQt1::Identity::List &identities,
                                               PolkitQt1::Agent::AsyncResult *result)
{
    qDebug() << "Initiating authentication";

    if (m_inProgress) {
        result->setError("Another client is already authenticating, please try again later.");
        result->setCompleted();
        qDebug() << "Another client is already authenticating, please try again later.";
        return;
    }

    m_identities = identities;
    m_cookie = cookie;
    m_result = result;
    m_session.clear();
    m_password.clear();

    m_inProgress = true;
    m_delayRemoveTimer.stop();

    WId parentId = 0;
    if (m_actionsToWID.contains(actionId)) {
        parentId = m_actionsToWID[actionId];
    }

    m_pluginManager.data()->setActionID(actionId);

    if (!m_dialog.isNull()) {
        m_dialog->deleteLater();
        m_dialog = nullptr;
    }

    m_dialog = new AuthDialog(actionId, message, iconName, details, identities, parentId);

    connect(m_dialog.data(), SIGNAL(okClicked()), SLOT(dialogAccepted()));
    connect(m_dialog.data(), SIGNAL(rejected()), SLOT(dialogCanceled()));
    connect(m_dialog.data(), SIGNAL(adminUserSelected(PolkitQt1::Identity)), SLOT(userSelected(PolkitQt1::Identity)));

    // TODO(hualet): show extended action information.

    QList<QButtonGroup *> optionsList = m_pluginManager.data()->reduceGetOptions(actionId);
    for (QButtonGroup *bg : optionsList) {
        m_dialog.data()->addOptions(bg);
    }

    qDebug() << "WinId of the dialog is " << m_dialog.data()->winId() << m_dialog.data()->effectiveWinId();
    m_dialog.data()->show();
    qDebug() << "WinId of the shown dialog is " << m_dialog.data()->winId() << m_dialog.data()->effectiveWinId();
    m_dialog.data()->activateWindow();

    if (identities.length() == 1) {
        m_selectedUser = identities[0];
    } else {
        m_selectedUser = m_dialog.data()->adminUserSelected();
    }

    m_numTries = 0;
    tryAgain();
}

void PolicyKitListener::tryAgain()
{
    qDebug() << "Trying again";
    m_wasCancelled = false;
    m_password.clear();

    qDebug() << m_selectedUser.isValid() << m_selectedUser.toString();

    // We will create new session only when some user is selected
    if (m_selectedUser.isValid()) {
        m_session = new Session(m_selectedUser, m_cookie, m_result);
        m_usePassword = false;

        connect(m_session.data(), &Session::request, this, &PolicyKitListener::request);
        connect(m_session.data(), &Session::completed, this, &PolicyKitListener::completed);
        connect(m_session.data(), &Session::showError, this, &PolicyKitListener::showError);
        connect(m_session.data(), &Session::showInfo, this, &PolicyKitListener::showInfo);

        const QString username { m_selectedUser.toString().replace("unix-user:", "") };

        m_session->initiate();

    }
}

void PolicyKitListener::finishObtainPrivilege()
{
    qDebug() << "Finishing obtaining privileges";

    // Number of tries increase only when some user is selected
    if (m_selectedUser.isValid() && m_usePassword) {
        m_numTries++;
    }

    if (!m_gainedAuthorization && !m_wasCancelled && !m_dialog.isNull()) {
        m_dialog.data()->authenticationFailure(m_numTries, m_usePassword);

        if (m_numTries < 3) {
            m_session.data()->deleteLater();

            tryAgain();
            return;
        }
    }

    // 插件进行的操作不应该能够长时间阻塞 UI 线程
    // 将插件操作放在新线程中完成的原因是
    // https://gerrit.uniontech.com/c/dpa-ext-gnomekeyring/+/45034/2/gnomekeyringextention.cpp#138 
    // 调用了一个可能会阻塞的方法, 导致了 pms bug 82328
    if (m_gainedAuthorization)
        QtConcurrent::run(m_pluginManager.data(),
                          &PluginManager::reduce,
                          m_selectedUser.toString().remove("unix-user:"),
                          m_dialog.data()->password());

    // fill complete according to authentication result
    // to get cancel state, polkit-qt need be updated
    fillResult();
    m_session.data()->deleteLater();
    if (!m_dialog.isNull()) {
        if (m_numTries >= 3 && !m_gainedAuthorization && !m_wasCancelled) {
            m_delayRemoveTimer.start();
        } else {
            m_dialog.data()->hide();
            QMetaObject::invokeMethod(m_dialog.data(), "deleteLater", Qt::QueuedConnection);
        }
    }
    m_inProgress = false;

    m_numFPrint = 0;
    m_usePassword = false;

    qDebug() << "Finish obtain authorization:" << m_gainedAuthorization;
}

bool PolicyKitListener::initiateAuthenticationFinish()
{
    qDebug() << "Finishing authentication";
    return true;
}

void PolicyKitListener::cancelAuthentication()
{
    qDebug() << "Cancelling authentication";

    m_wasCancelled = true;
    finishObtainPrivilege();
}

void PolicyKitListener::request(const QString &request, bool echo)
{
    Q_UNUSED(echo);

    qDebug() << "Request: " << request;

    if (m_dialog && !request.isEmpty())
        m_dialog.data()->setAuthInfo(request);

}

void PolicyKitListener::completed(bool gainedAuthorization)
{
    qDebug() << "Completed: " << gainedAuthorization;

    m_gainedAuthorization = gainedAuthorization;
    if (m_showInfoSuccess)
        m_gainedAuthorization = true;
    m_showInfoSuccess = false;

    finishObtainPrivilege();
}

void PolicyKitListener::showError(const QString &text)
{
    qDebug() << "Error: " << text;

    if (m_dialog && !text.isEmpty())
        m_dialog.data()->setError(text);
}

void PolicyKitListener::showInfo(const QString &info)
{
    qDebug() << "Info: " << info;

    if (m_dialog && !info.isEmpty())
        m_dialog.data()->setAuthInfo(info);

    if ("Verification successful" == info)
        m_showInfoSuccess = true;
    else
        m_showInfoSuccess = false;
}

bool PolicyKitListener::isDeepin()
{
    bool is_deepin = true;
    if (m_gsettings != nullptr && m_gsettings->keys().contains(USE_DEEPIN_AUTH)) {
        is_deepin = m_gsettings->get(USE_DEEPIN_AUTH).toBool();
    }
    return is_deepin;
}

void PolicyKitListener::dialogAccepted()
{
    m_delayRemoveTimer.stop();
    if (!m_dialog.isNull()) {
        qDebug() << "Dialog accepted";

        m_usePassword = true;
        m_password = m_dialog->password();
        m_session->setResponse(m_password);
    }
}

void PolicyKitListener::dialogCanceled()
{
    qDebug() << "Dialog cancelled";

    m_inProgress = false;
    m_delayRemoveTimer.stop();
    m_wasCancelled = true;
    if (!m_session.isNull()) {
        m_session.data()->cancel();
    }

    finishObtainPrivilege();
}

void PolicyKitListener::userSelected(const PolkitQt1::Identity &identity)
{
    m_selectedUser = identity;
    // If some user is selected we must destroy existing session
    if (!m_session.isNull()) {
        m_session.data()->deleteLater();
    }
    tryAgain();
}

void PolicyKitListener::fillResult()
{
    if (!m_session.isNull()) {
        if (m_wasCancelled) {
            m_session.data()->result()->setCancel("aciton cancel");
        } else if (!m_gainedAuthorization) {
            m_session.data()->result()->setError("password error");
        }
        m_session.data()->result()->setCompleted();
    } else {
        if (m_wasCancelled) {
            m_result->setCancel("action cancel");
        } else if (!m_gainedAuthorization) {
            m_result->setError("password error");
        }
        m_result->setCompleted();
    }
}

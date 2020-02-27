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

#include <polkit-qt5-1/PolkitQt1/Agent/Listener>
#include <polkit-qt5-1/PolkitQt1/Agent/Session>
#include <polkit-qt5-1/PolkitQt1/Subject>
#include <polkit-qt5-1/PolkitQt1/Identity>
#include <polkit-qt5-1/PolkitQt1/Details>

#include "policykitlistener.h"
#include "AuthDialog.h"

#include "polkit1authagent_adaptor.h"

#include "pluginmanager.h"

//#include "libdde-auth/deepinauthframework.h"

PolicyKitListener::PolicyKitListener(QObject *parent)
        : Listener(parent)
        , m_selectedUser(0)
        , m_inProgress(false)
        , m_usePassword(false)
        , m_numFPrint(0)
{
    (void) new Polkit1AuthAgentAdaptor(this);

//    m_deepinAuthFramework = new DeepinAuthFramework(this, this);
//    m_fprintdInter = new FPrintd("com.deepin.daemon.Fprintd", "/com/deepin/daemon/Fprintd",
//                                 QDBusConnection::systemBus(), this);

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

//    connect(m_fprintdInter, &FPrintd::DevicesChanged, this, &PolicyKitListener::fprintdDeviceChanged);
//    fprintdDeviceChanged();
}

//void PolicyKitListener::fprintdDeviceChanged()
//{
//    if (m_fprintdDeviceInter) {
//        m_fprintdDeviceInter->deleteLater();
//        m_fprintdDeviceInter = nullptr;
//    }

//    QDBusObjectPath default_fprintd_device_path = m_fprintdInter->GetDefaultDevice();
//    if (!default_fprintd_device_path.path().isEmpty()) {
//        m_fprintdDeviceInter = new FPrintdDevice("com.deepin.daemon.Fprintd",
//                                                 default_fprintd_device_path.path(),
//                                                 QDBusConnection::systemBus(), this);
//    }

//    tryAgain();
//}

PolicyKitListener::~PolicyKitListener()
{
}

void PolicyKitListener::setWIdForAction(const QString &action, qulonglong wID)
{
    qDebug() << "On to the handshake";
    m_actionsToWID[action] = wID;
}

void PolicyKitListener::onDisplayErrorMsg(const QString &errtype, const QString &msg)
{
    if (!m_dialog.isNull()) {
        if (errtype == "verify-timed-out")
            m_dialog->setAuthMode(AuthDialog::AuthMode::Password);
        m_dialog->setError(msg);
    }
}

void PolicyKitListener::onDisplayTextInfo(const QString &msg)
{
    if (!m_dialog.isNull()) {
        m_dialog->setRequest(msg, true);
    }
}

void PolicyKitListener::onPasswordResult(const QString &msg)
{
    if (msg.isEmpty()) {
        return completed(false);
    }

    m_password = msg;
    m_session->initiate();
    m_session->setResponse(m_password);
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

        connect(m_session.data(), &Session::request, this, &PolicyKitListener::request);
        connect(m_session.data(), SIGNAL(completed(bool)), this, SLOT(completed(bool)));
        connect(m_session.data(), SIGNAL(showError(QString)), this, SLOT(showError(QString)));

        const QString username { m_selectedUser.toString().replace("unix-user:", "") };

        m_session->initiate();
#ifdef ENABLE_DEEPIN_AUTH
        bool hasFingers = false;
        if (m_fprintdDeviceInter) {
            QDBusPendingReply<QStringList> rep = m_fprintdDeviceInter->ListEnrolledFingers(username);
            rep.waitForFinished();
            if (rep.isValid() && !rep.value().isEmpty())
                hasFingers = true;
        }

        m_dialog->setAuthMode(hasFingers ? AuthDialog::FingerPrint : AuthDialog::Password);

        m_deepinAuthFramework->Clear();
        m_deepinAuthFramework->SetUser(username);
        m_deepinAuthFramework->Authenticate();
#else
        m_dialog->setAuthMode(AuthDialog::Password);
#endif
    }
}

void PolicyKitListener::finishObtainPrivilege()
{
    qDebug() << "Finishing obtaining privileges";

    // Number of tries increase only when some user is selected
    if (m_selectedUser.isValid()) {
        m_numTries++;
    }

    if (!m_gainedAuthorization && !m_wasCancelled && !m_dialog.isNull()) {
        m_dialog.data()->authenticationFailure();

        if (m_numTries < 3) {
            m_session.data()->deleteLater();

            tryAgain();
            return;
        }
    }

    if (m_gainedAuthorization)
        m_pluginManager.data()->reduce(m_selectedUser.toString().remove("unix-user:"),
                                       m_dialog.data()->password());

    if (!m_session.isNull()) {
        m_session.data()->result()->setCompleted();
    } else {
        m_result->setCompleted();
    }
    m_session.data()->deleteLater();

    if (!m_dialog.isNull()) {
        m_dialog.data()->hide();

        // FIXME(hualet): don't know why deleteLater doesn't do its job,
        // combined invokeMethod with Qt::QueuedConnection works well.
        // m_dialog.data()->deleteLater();
        QMetaObject::invokeMethod(m_dialog.data(), "deleteLater", Qt::QueuedConnection);
    }

    m_inProgress = false;

    m_numFPrint = 0;
    m_usePassword = false;

//    m_deepinAuthFramework->Clear();

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
//    while(m_password.isEmpty()) {
//        sleep(10);
//    }

//    if (!m_dialog.isNull()) {
//        m_dialog.data()->setRequest(request, m_selectedUser.isValid() &&
//                m_selectedUser.toString() == "unix-user:root");

//        if (request.simplified().remove(":") == "Password") {
//            m_session.data()->setResponse(m_password);
//        }
//    }
}

void PolicyKitListener::completed(bool gainedAuthorization)
{
    qDebug() << "Completed: " << gainedAuthorization;

    m_gainedAuthorization = gainedAuthorization;

    finishObtainPrivilege();
}

void PolicyKitListener::showError(const QString &text)
{
    qDebug() << "Error: " << text;

    m_dialog.data()->setError(text);
}

void PolicyKitListener::dialogAccepted()
{
    if (!m_dialog.isNull()) {
        qDebug() << "Dialog accepted";
#ifdef ENABLE_DEEPIN_AUTH
        m_deepinAuthFramework->Clear();
        m_deepinAuthFramework->SetUser(m_selectedUser.toString().remove("unix-user:"));
        m_deepinAuthFramework->setPassword(m_dialog->password());
        m_deepinAuthFramework->Authenticate();
#else
        m_password = m_dialog->password();
        m_session->setResponse(m_password);
#endif
    }
}

void PolicyKitListener::dialogCanceled()
{
    qDebug() << "Dialog cancelled";

    m_wasCancelled = true;
    if (!m_session.isNull()) {
        m_session.data()->cancel();
//        m_deepinAuthFramework->Clear();
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

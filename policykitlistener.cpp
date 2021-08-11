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
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <polkit-qt5-1/PolkitQt1/Agent/Listener>
#include <polkit-qt5-1/PolkitQt1/Agent/Session>
#include <polkit-qt5-1/PolkitQt1/Subject>
#include <polkit-qt5-1/PolkitQt1/Identity>
#include <polkit-qt5-1/PolkitQt1/Details>
#include <polkit-qt5-1/polkitqt1-version.h>
#include <libintl.h>

#include "policykitlistener.h"
#include "AuthDialog.h"

#include "polkit1authagent_adaptor.h"

#include "pluginmanager.h"

#define USE_DEEPIN_AUTH "useDeepinAuth"

static bool isAccountLocked(const PolkitQt1::Identity &identity);

PolicyKitListener::PolicyKitListener(QObject *parent)
    : Listener(parent)
    , m_selectedUser(nullptr)
    , m_inProgress(false)
    , m_gainedAuthorization(false)
    , m_wasCancelled(false)
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
    m_wasCancelled = false;
    m_inProgress = true;

    WId parentId = 0;
    if (m_actionsToWID.contains(actionId)) {
        parentId = m_actionsToWID[actionId];
    }

    m_pluginManager.data()->setActionID(actionId);

    m_dialog = new AuthDialog(actionId, message, iconName, details, identities, parentId);
    m_dialog->setAttribute(Qt::WA_DeleteOnClose);
    initDialog(actionId);

    if (identities.length() == 1) {
        createSessionForId(identities[0]);
    } else {
        createSessionForId(m_dialog.data()->selectedAdminUser());
    }
}

void PolicyKitListener::initDialog(const QString &actionId)
{
    connect(m_dialog.data(), &AuthDialog::okClicked, this, &PolicyKitListener::dialogAccepted);
    connect(m_dialog.data(), &AuthDialog::rejected, this, &PolicyKitListener::dialogCanceled);
    connect(m_dialog.data(), &AuthDialog::adminUserSelected, this, &PolicyKitListener::createSessionForId);

    // TODO(hualet): show extended action information.
    QList<QButtonGroup *> optionsList = m_pluginManager.data()->reduceGetOptions(actionId);
    for (QButtonGroup *bg : optionsList) {
        m_dialog.data()->addOptions(bg);
    }

    qDebug() << "WinId of the dialog is " << m_dialog.data()->winId() << m_dialog.data()->effectiveWinId();
    m_dialog.data()->show();
    qDebug() << "WinId of the shown dialog is " << m_dialog.data()->winId() << m_dialog.data()->effectiveWinId();
    m_dialog.data()->activateWindow();
}

void PolicyKitListener::finishObtainPrivilege()
{
    qDebug() << "Finishing obtaining privileges";

    // 插件进行的操作不应该能够长时间阻塞 UI 线程
    // 将插件操作放在新线程中完成的原因是
    // https://gerrit.uniontech.com/c/dpa-ext-gnomekeyring/+/45034/2/gnomekeyringextention.cpp#138
    // 调用了一个可能会阻塞的方法, 导致了 pms bug 82328
    if (m_gainedAuthorization) {
        QtConcurrent::run(m_pluginManager.data(),
                          &PluginManager::reduce,
                          m_selectedUser.toString().remove("unix-user:"),
                          m_dialog.data()->password());
    } else if (!m_wasCancelled) {
        // 认证失败
        m_dialog->authenticationFailure();
        if (isAccountLocked(m_selectedUser)) {
            m_dialog->lock(); // 锁定
        } else {
            createSessionForId(m_selectedUser); // 重试
        }
        return;
    }

    // fill complete according to authentication result
    // to get cancel state, polkit-qt need be updated
    fillResult();
    m_session.data()->deleteLater();
    m_dialog->close();

    m_inProgress = false;

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
    qDebug() << "Dialog accepted";
    m_session->setResponse(m_dialog->password());
}

void PolicyKitListener::dialogCanceled()
{
    qDebug() << "Dialog cancelled";

    m_inProgress = false;
    m_wasCancelled = true;
    if (!m_session.isNull()) {
        m_session.data()->cancel();
    }
    finishObtainPrivilege();
}

void PolicyKitListener::createSessionForId(const PolkitQt1::Identity &identity)
{
    m_inProgress = true;
    m_selectedUser = identity;
    // If some user is selected we must destroy existing session
    if (!m_session.isNull()) {
        m_session.data()->deleteLater();
    }
    // We will create new session only when some user is selected
    if (m_selectedUser.isValid()) {
        m_session = new Session(m_selectedUser, m_cookie, m_result);

        connect(m_session.data(), &Session::request, this, &PolicyKitListener::request);
        connect(m_session.data(), &Session::completed, this, &PolicyKitListener::completed);
        connect(m_session.data(), &Session::showError, this, &PolicyKitListener::showError);
        connect(m_session.data(), &Session::showInfo, this, &PolicyKitListener::showInfo);

        m_session->initiate();
    }
}

void PolicyKitListener::fillResult()
{
    PolkitQt1::Agent::AsyncResult *result = m_session.isNull() ? m_result : m_session->result();
    if (result) {
        //polkit-qt-1在0.115.0版本新增setCancel接口
#if POLKITQT1_IS_VERSION(0, 115, 0)
        if (m_wasCancelled) {
            result->setCancel("action cancel");
        } else if (!m_gainedAuthorization) {
            result->setError("password error");
        }
#endif
        result->setCompleted();
    }
}

static bool isAccountLocked(const PolkitQt1::Identity &identity)
{
    QString userName = identity.toString().replace("unix-user:", "");
    QDBusMessage msg = QDBusMessage::createMethodCall("com.deepin.daemon.Authenticate",
                                                      "/com/deepin/daemon/Authenticate",
                                                      "com.deepin.daemon.Authenticate",
                                                      "GetLimits");
    msg << userName;
    msg = QDBusConnection::systemBus().call(msg, QDBus::Block, 3000);
    QJsonDocument document;
    if (QDBusMessage::ReplyMessage == msg.type()) {
        document = QJsonDocument::fromJson(msg.arguments().at(0).toString().toUtf8());
    }
    QJsonArray array = document.array();

    bool result = false;
    for (auto item = array.constBegin(); item != array.constEnd(); item++) {
        // 后续可以支持多种认证方式：fingerprint, face, usbkey 等
        if (item->toObject()["type"].toString() == "password") {
            result = item->toObject()["locked"].toBool();
            break;
        }
    }
    return result;
}

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

#include "AuthDialog.h"
#include "accessiblemap.h"
#include "usersmanager.h"

#include <QProcess>
#include <QPainter>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QUrl>
#include <QAbstractButton>
#include <QButtonGroup>

#include <DHiDPIHelper>

#include <PolkitQt1/Authority>
#include <PolkitQt1/Details>

#include <libintl.h>

DWIDGET_USE_NAMESPACE

AuthDialog::AuthDialog(const QString &actionId,
                       const QString &message,
                       const QString &iconName,
                       const PolkitQt1::Details &details,
                       const PolkitQt1::Identity::List &identities,
                       WId parent)
    : DDialog(message, "", nullptr)
    , m_message(message)
    , m_iconName(iconName)
    , m_adminsCombo(new QComboBox(this))
    , m_passwordInput(new DPasswordEdit(this))
    , m_tooltip(new ErrorTooltip(""))
    , m_numTries(0)
    , m_lockLimitTryNum(getLockLimitTryNum())
{
    Q_UNUSED(details)
    Q_UNUSED(parent)

    // TODO: associate this dialog with its parent.
    setupUI();

    setlocale(LC_ALL, "");

    // find action description for actionId
    foreach (const PolkitQt1::ActionDescription &desc, PolkitQt1::Authority::instance()->enumerateActionsSync()) {
        if (actionId == desc.actionId()) {
            m_actionDescription = desc;
            qDebug() << "Action description has been found" ;
            break;
        }
    }

    qDebug() << "lock limit: " << m_lockLimitTryNum;

    // 始终显示用户名 (bug:9145,降低用户理解成本)
    connect(m_adminsCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_userCB_currentIndexChanged(int)));

    connect(this, &AuthDialog::aboutToClose, this, &AuthDialog::rejected);
    connect(this, &AuthDialog::clearAccessibleMap, AccessibleMap::instance(),&AccessibleMap::clearAccessibleMap,Qt::DirectConnection);
}

AuthDialog::~AuthDialog()
{
    qDebug() << "~AuthDialog";
    m_tooltip->hide();
    delete m_tooltip;
    Q_EMIT clearAccessibleMap();
}

void AuthDialog::setError(const QString &error)
{
    //由于无法获取到dgetText的临时方案
    QString dgetText = "";
    if ("Finger moved too fast, please do not lift until prompted" == error) {
        dgetText = tr("Finger moved too fast, please do not lift until prompted");
    } else if ("Verification failed, 2 chances left" == error) {
        dgetText = tr("Verification failed, two chances left");
    } else {
        dgetText = QString(dgettext("deepin-authentication", error.toUtf8()));
    }
    m_errorMsg = dgetText;
}

void AuthDialog::setRequest(const QString &request, bool requiresAdmin)
{
    Q_UNUSED(request)
    Q_UNUSED(requiresAdmin)
}

void AuthDialog::setAuthInfo(const QString &info)
{
    if ("Password" == info)
        m_passwordInput->lineEdit()->setFocus();

    m_passwordInput->lineEdit()->setPlaceholderText(QString(dgettext("deepin-authentication", info.toStdString().c_str())));

    setButtonText(1, tr("Confirm", "button"));
    getButton(1)->setAccessibleName("Confirm");
    update();
}

void AuthDialog::addOptions(QButtonGroup *bg)
{
    QList<QAbstractButton *> btns = bg->buttons();

    if (btns.length() > 0) {
        addSpacing(10);
    }

    for (QAbstractButton *btn : btns) {
        addContent(btn);
    }
}

void AuthDialog::createUserCB(const PolkitQt1::Identity::List &identities)
{
    // Clears the combobox in the case some user be added
    m_adminsCombo->clear();

    // For each user
    foreach (const PolkitQt1::Identity &identity, identities) {
        if (!identity.isValid()) {
            continue;
        }

        // appends the user item
        QString username = identity.toString().remove("unix-user:");
        QString fullname = UsersManager::instance()->getFullName(username);

        if (username == qgetenv("USER"))
            m_adminsCombo->insertItem(0, fullname.isEmpty() ? username : fullname, identity.toString());
        else
            m_adminsCombo->addItem(fullname.isEmpty() ? username : fullname, identity.toString());
    }

    m_adminsCombo->setCurrentIndex(0); // select the current user.
    m_adminsCombo->show();
}

void AuthDialog::showErrorTip()
{
    QPoint globalStart = mapToGlobal(m_passwordInput->pos());
    m_tooltip->show(globalStart.x(),
                    globalStart.y() + m_passwordInput->height());
}

PolkitQt1::Identity AuthDialog::selectedAdminUser() const
{
    qDebug() << m_adminsCombo->currentIndex() << m_adminsCombo->currentData().toString();

    if (m_adminsCombo->currentIndex() == -1)
        return PolkitQt1::Identity();

    const QString id = m_adminsCombo->currentData().toString();
    if (id.isEmpty())
        return PolkitQt1::Identity();
    return PolkitQt1::Identity::fromString(id);
}

int AuthDialog::getLockLimitTryNum()
{
    const QString path = "/usr/share/dde-session-shell/dde-session-shell.conf";
    int count = 5;
    QFile file(path);
    if (!file.exists()) {
        return count;
    }

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("LockTime");
    count = settings.value("lockLimitTryNum").toInt();
    settings.endGroup();
    return count;
}

void AuthDialog::on_userCB_currentIndexChanged(int /*index*/)
{
    PolkitQt1::Identity identity = selectedAdminUser();
    // 清除上一个用户已经输入的密码
    m_passwordInput->clear();
    m_passwordInput->setAlert(false);
    m_errorMsg = "";
    m_numTries = 0;

    // itemData is Null when "Select user" is selected
    if (!identity.isValid()) {
        // 清理警告信息
        m_tooltip->hide();
        m_passwordInput->setEnabled(false);
    } else {
        // 判断用户密码是否在有效期内
        QDBusInterface accounts("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", "com.deepin.daemon.Accounts", QDBusConnection::systemBus());
        const QString &path = accounts.call("FindUserById", QString::number(identity.toUnixUserIdentity().uid())).arguments().value(0).toString();
        bool passwordIsExpired = false;

        if (!path.isEmpty()) {
            QDBusInterface accounts_user("com.deepin.daemon.Accounts", path, "com.deepin.daemon.Accounts.User", QDBusConnection::systemBus());
            passwordIsExpired = accounts_user.call("IsPasswordExpired").arguments().value(0).toBool();
        }

        // 如果密码以过期
        if (passwordIsExpired) {
            m_passwordInput->setEnabled(false);
            setError(tr("You are required to change your password immediately (password expired)"));
        } else {
            // 清理警告信息
            m_tooltip->hide();
            m_passwordInput->setEnabled(true);
            // We need this to restart the auth with the new user
            emit adminUserSelected(identity);
            // git password label focus
            m_passwordInput->lineEdit()->setFocus();
        }
    }
}

void AuthDialog::showEvent(QShowEvent *event)
{
    if (!m_tooltip->text().isEmpty()) {
        // 确保错误信息能正常显示
        QTimer::singleShot(500, this, &AuthDialog::showErrorTip);
    }

    return DDialog::showEvent(event);
}

void AuthDialog::hideEvent(QHideEvent *event)
{
    m_tooltip->hide();
    DDialog::hideEvent(event);
}

void AuthDialog::moveEvent(QMoveEvent *event)
{
    DDialog::moveEvent(event);

    if (m_tooltip->isVisible()) {
        showErrorTip();
    }
}

void AuthDialog::focusInEvent(QFocusEvent *event)
{
    DDialog::focusInEvent(event);

    if (m_tooltip->isVisible()) {
        m_tooltip->hide();
    }
}

QString AuthDialog::password() const
{
    return m_passwordInput->text();
}

void AuthDialog::lock()
{
    m_passwordInput->setEnabled(false);
    getButton(1)->setEnabled(false);
}

void AuthDialog::authenticationFailure(bool &isLock)
{
    m_numTries++;
    if (!isLock) {
        // 不存在DA的情况，由次数来判定是否锁定
        if (m_lockLimitTryNum <= m_numTries) {
            isLock = true;
        }
    }

    if (m_errorMsg.isEmpty()) {
        // 专业版错误信息现在由DA提供，考虑没有DA的版本，保留以前由agent提供错误的方案
        qDebug() << "authentication failed, error message is empty, set error message by agent.";
        if (isLock) {
            setError(tr("Locked, please try again later"));
        } else {
            setError(tr("Wrong password"));
        }
    }

    m_passwordInput->setEnabled(true);
    m_passwordInput->showAlertMessage(m_errorMsg);
    m_passwordInput->setAlert(true);
    m_passwordInput->clear();
    m_passwordInput->lineEdit()->setFocus();

    if (isLock) {
        lock();
    }
    activateWindow();
}

void AuthDialog::setupUI()
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::Tool);
    setMinimumWidth(380);

    m_adminsCombo->setAccessibleName("selectuser");
    m_passwordInput->setAccessibleName("passwordinput");

    int cancelId = addButton(tr("Cancel", "button"));
    int confirmId = addButton(tr("Confirm", "button"), true, ButtonType::ButtonRecommend);
    getButton(confirmId)->setEnabled(false);

    setOnButtonClickedClose(false);
    setDefaultButton(1);

    getButton(cancelId)->setAccessibleName("Cancel");
    getButton(confirmId)->setAccessibleName("Confirm");

    m_passwordInput->setAccessibleName("PasswordInput");
    m_adminsCombo->setAccessibleName("AdminUsers");

    connect(this, &AuthDialog::buttonClicked, [this](int index, QString) {
        switch (index) {
        case 0:
            emit rejected();
            break;
        case 1: {
            emit okClicked();
            break;
        }
        default:;
        }
    });

    connect(m_passwordInput, &DPasswordEdit::textChanged, [ = ](const QString & text) {
        getButton(confirmId)->setEnabled(text.length() > 0);
        if (text.length() == 0) return;

        m_tooltip->hide();
        m_passwordInput->setAlert(false);
        m_errorMsg = "";
    });

    QPixmap icon;

    const qreal dpr = devicePixelRatioF();
    if (!m_iconName.isEmpty() && QIcon::hasThemeIcon(m_iconName)) {
        icon = QIcon::fromTheme(m_iconName).pixmap(static_cast<int>(48 * dpr), static_cast<int>(48 * dpr));
    } else {
        icon = DHiDPIHelper::loadNxPixmap(":/images/default.svg");
    }

    icon.setDevicePixelRatio(dpr);
    setIcon(icon);

    m_adminsCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_adminsCombo->hide();
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_tooltip->hide();

    addSpacing(10);
    addContent(m_adminsCombo);
    addSpacing(6);
    addContent(m_passwordInput);
}

AuthDetails::AuthDetails(const PolkitQt1::Details &details,
                         const PolkitQt1::ActionDescription &actionDescription,
                         const QString &appname,
                         QWidget *parent)
    : QWidget(parent)
{
    Q_UNUSED(details)
    Q_UNUSED(actionDescription)
    Q_UNUSED(appname)

    /*
    setupUi(this);

    app_label->setText(appname);

    foreach(const QString &key, details.keys()) { //krazy:exclude=foreach (Details is not a map/hash, but rather a method)
        int row = gridLayout->rowCount() + 1;

        QLabel *keyLabel = new QLabel(this);
        keyLabel->setText(i18nc("%1 is the name of a detail about the current action "
                                "provided by polkit", "%1:", key));
        gridLayout->addWidget(keyLabel, row, 0);

        QLabel *valueLabel = new QLabel(this);
        valueLabel->setText(details.lookup(key));
        gridLayout->addWidget(valueLabel, row, 1);
    }

    action_label->setText(actionDescription.description());

    action_label->setTipText(tr("Click to edit %1", actionDescription.actionId()));
    action_label->setUrl(actionDescription.actionId());

    QString vendor    = actionDescription.vendorName();
    QString vendorUrl = actionDescription.vendorUrl();

    if (!vendor.isEmpty()) {
        vendorUL->setText(vendor);
        vendorUL->setTipText(tr("Click to open %1", vendorUrl));
        vendorUL->setUrl(vendorUrl);
    } else if (!vendorUrl.isEmpty()) {
        vendorUL->setText(vendorUrl);
        vendorUL->setTipText(tr("Click to open %1", vendorUrl));
        vendorUL->setUrl(vendorUrl);
    } else {
        vendorL->hide();
        vendorUL->hide();
    }

    connect(vendorUL, SIGNAL(leftClickedUrl(QString)), SLOT(openUrl(QString)));
    connect(action_label, SIGNAL(leftClickedUrl(QString)), SLOT(openAction(QString)));
     */
}

void AuthDetails::openUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void AuthDetails::openAction(const QString &url)
{
    // FIXME what's this? :)
    QProcess::startDetached("polkit-kde-authorization", QStringList() << url);
}


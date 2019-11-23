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

#include "usersmanager.h"

AuthDialog::AuthDialog(const QString &actionId,
                       const QString &message,
                       const QString &iconName,
                       const PolkitQt1::Details &details,
                       const PolkitQt1::Identity::List &identities,
                       WId parent)
    : DDialog(message, "", nullptr),
      m_message(message),
      m_iconName(iconName),
      m_iconLabel(new QLabel(this)),
      m_adminsCombo(new QComboBox(this)),
      m_passwordInput(new DPasswordEdit(this)),
      m_tooltip(new ErrorTooltip("")),
      m_fprintTip(new DLineEdit(this)),
      m_currentAuthMode(AuthMode::FingerPrint)
{
    Q_UNUSED(details)
    Q_UNUSED(parent)

    // TODO: associate this dialog with its parent.
    setupUI();

    setlocale(LC_ALL, "");

    // find action description for actionId
    foreach(const PolkitQt1::ActionDescription &desc, PolkitQt1::Authority::instance()->enumerateActionsSync()) {
        if (actionId == desc.actionId()) {
            m_actionDescription = desc;
            qDebug() << "Action description has been found" ;
            break;
        }
    }

    // If there is more than 1 identity we will show the combobox for user selection
    if (identities.size() > 1) {
        connect(m_adminsCombo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(on_userCB_currentIndexChanged(int)));

        createUserCB(identities);
    } else {
        m_adminsCombo->addItem("", identities[0].toString());
        m_adminsCombo->setCurrentIndex(0);
    }

    QString text = "Password: ";
    m_passwordInput->lineEdit()->setPlaceholderText(QString(dgettext("Linux-PAM", text.toStdString().c_str())));

    connect(this, &AuthDialog::aboutToClose, this, &AuthDialog::rejected);
}

AuthDialog::~AuthDialog()
{
    qDebug() << "~AuthDialog";
    m_tooltip->hide();
    m_tooltip->deleteLater();
}

void AuthDialog::setError(const QString &error)
{
    m_tooltip->setMessage(error);
    showErrorTip();
}

void AuthDialog::setRequest(const QString &request, bool requiresAdmin)
{
    Q_UNUSED(requiresAdmin)

    // FIXME(sbw):
    // Let's Do some evil here:
    // in current environment, Don't know why but request text "Password: "
    // missing last character ' '. so the translated message not load currectly.
    // This really is a bug.
    if (request.startsWith("Password:")) {
        setAuthMode(AuthMode::Password);
    }
}

AuthDialog::AuthMode AuthDialog::authMode()
{
    return m_currentAuthMode;
}

void AuthDialog::setAuthMode(AuthDialog::AuthMode mode)
{
    switch (mode) {
    case AuthMode::FingerPrint: {
        m_fprintTip->show();
        m_passwordInput->hide();
        m_currentAuthMode = AuthMode::FingerPrint;
        m_fprintTip->lineEdit()->setPlaceholderText(QString(tr("Verify your fingerprint or password")));
        break;
    }
    case AuthMode::Password: {
        m_fprintTip->hide();
        m_passwordInput->show();
        m_passwordInput->lineEdit()->setFocus();
        m_currentAuthMode = AuthMode::Password;

        Q_EMIT usePassword();

        break;
    }
    default:
        break;
    }

    setButtonText(1, tr("Confirm"));
    getButton(1)->setAccessibleName("Confirm");
}

void AuthDialog::addOptions(QButtonGroup *bg)
{
    QList<QAbstractButton*> btns = bg->buttons();

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
    foreach(const PolkitQt1::Identity &identity, identities) {
        if (!identity.isValid()) {
            continue;
        }

        // appends the user item
        QString username = identity.toString().remove("unix-user:");

        QString fullname = UsersManager::instance()->getFullName(username);
        if (!fullname.isEmpty()) username = fullname;

        if (username == qgetenv("USER"))
            m_adminsCombo->insertItem(0, username, identity.toString());
        else
            m_adminsCombo->addItem(username, identity.toString());

        // TODO: select the current user.
    }

    m_adminsCombo->setCurrentIndex(0);

    m_adminsCombo->show();
}

void AuthDialog::showErrorTip()
{
    QPoint globalStart;
    if (m_currentAuthMode == AuthMode::FingerPrint) {
        globalStart = mapToGlobal(m_fprintTip->pos());
    } else if (m_currentAuthMode == AuthMode::Password) {
        globalStart = mapToGlobal(m_passwordInput->pos());
    }
    m_tooltip->show(globalStart.x(),
                    globalStart.y() + m_passwordInput->height());
}

PolkitQt1::Identity AuthDialog::adminUserSelected() const
{
    qDebug() << m_adminsCombo->currentIndex() << m_adminsCombo->currentData().toString();

    if (m_adminsCombo->currentIndex() == -1)
        return PolkitQt1::Identity();

    const QString id = m_adminsCombo->currentData().toString();
    if (id.isEmpty())
        return PolkitQt1::Identity();
    return PolkitQt1::Identity::fromString(id);
}

void AuthDialog::on_userCB_currentIndexChanged(int /*index*/)
{
    PolkitQt1::Identity identity = adminUserSelected();
    // itemData is Null when "Select user" is selected
    if (!identity.isValid()) {
        m_passwordInput->setEnabled(false);
    } else {
        m_passwordInput->setEnabled(true);
        // We need this to restart the auth with the new user
        emit adminUserSelected(identity);
        // git password label focus
        m_passwordInput->lineEdit()->setFocus();
    }
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
    return m_passwordInput->isVisible()
               ? m_passwordInput->text()
               : m_fprintTip->text();
}

void AuthDialog::authenticationFailure()
{
    // TODO: show error messages.
    m_passwordInput->setEnabled(true);
    m_passwordInput->setAlert(true);
    m_passwordInput->clear();
    m_passwordInput->lineEdit()->setFocus();

    setError(tr("Wrong password"));

    activateWindow();
}

void AuthDialog::setupUI()
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setMaximumWidth(380);

    int cancelId = addButton(tr("Cancel"));
    int confirmId = addButton(tr("Confirm"), true, ButtonType::ButtonRecommend);
    getButton(confirmId)->setEnabled(false);

    setOnButtonClickedClose(false);
    setDefaultButton(1);

    getButton(cancelId)->setAccessibleName("Cancel");
    getButton(confirmId)->setAccessibleName("Confirm");

    m_passwordInput->setAccessibleName("PasswordInput");
    m_adminsCombo->setAccessibleName("AdminUsers");
    m_fprintTip->setAccessibleName("FingerPrintInput");

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
    });

    QPixmap icon;

    const qreal dpr = devicePixelRatioF();
    if (!m_iconName.isEmpty() && QIcon::hasThemeIcon(m_iconName)) {
        icon = QIcon::fromTheme(m_iconName).pixmap(48 * dpr, 48 * dpr);
    } else {
        icon = DHiDPIHelper::loadNxPixmap(":/images/default.svg");
    }

    icon.setDevicePixelRatio(dpr);
    setIconPixmap(icon);

    m_adminsCombo->hide();
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->hide();
    m_tooltip->hide();
    m_fprintTip->setEnabled(true);

    addSpacing(10);
    addContent(m_adminsCombo);
    addSpacing(6);
    addContent(m_passwordInput);
    addContent(m_fprintTip);
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

void AuthDetails::openUrl(const QString& url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void AuthDetails::openAction(const QString &url)
{
    // FIXME what's this? :)
    QProcess::startDetached("polkit-kde-authorization", QStringList() << url);
}


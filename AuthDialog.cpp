#include "AuthDialog.h"

#include <QProcess>
#include <QPainter>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QUrl>

#include <PolkitQt1/Authority>
#include <PolkitQt1/Details>

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
      m_adminsCombo(new DComboBox(this)),
      m_passwordInput(new DPasswordEdit(this)),
      m_tooltip(new ErrorTooltip(tr("Wrong password")))
{
    Q_UNUSED(details)
    Q_UNUSED(parent)

    // TODO: associate this dialog with its parent.
    setupUI();

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
}

AuthDialog::~AuthDialog()
{
    qDebug() << "~AuthDialog";
    m_tooltip->hide();
    m_tooltip->deleteLater();
}

void AuthDialog::setRequest(const QString &request, bool requiresAdmin)
{
    Q_UNUSED(request)
    Q_UNUSED(requiresAdmin)

    /*
    qDebug() << request;
    PolkitQt1::Identity identity = adminUserSelected();
    if (request.startsWith(QLatin1String("password:"), Qt::CaseInsensitive)) {
        if (requiresAdmin) {
            if (!identity.isValid()) {
                lblPassword->setText(tr("Password for root:"));
            } else {
                lblPassword->setText(tr("Password for %1:",
                                          identity.toString().remove("unix-user:")));
            }
        } else {
            lblPassword->setText(tr("Password:"));
        }
    } else if (request.startsWith(QLatin1String("password or swipe finger:"),
                                  Qt::CaseInsensitive)) {
        if (requiresAdmin) {
            if (!identity.isValid()) {
                lblPassword->setText(tr("Password or swipe finger for root:"));
            } else {
                lblPassword->setText(tr("Password or swipe finger for %1:",
                                          identity.toString().remove("unix-user:")));
            }
        } else {
            lblPassword->setText(tr("Password or swipe finger:"));
        }
    } else {
        lblPassword->setText(request);
    }
    */
}

void AuthDialog::setOptions()
{
    /*
    lblContent->setText(tr("An application is attempting to perform an action that requires privileges."
                             " Authentication is required to perform this action."));
    */
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
            m_adminsCombo->addItem(username, identity.toString());

            // TODO: select the current user.
        }

    m_adminsCombo->show();
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
    /*
    PolkitQt1::Identity identity = adminUserSelected();
    // itemData is Null when "Select user" is selected
    if (!identity.isValid()) {
        m_passwordInput->setEnabled(false);
        lblPassword->setEnabled(false);
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        m_passwordInput->setEnabled(true);
        lblPassword->setEnabled(true);
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        // We need this to restart the auth with the new user
        emit adminUserSelected(identity);
        // git password label focus
        m_passwordInput->setFocus();
    }
     */
}

QString AuthDialog::password() const
{
    return m_passwordInput->text();
}

void AuthDialog::authenticationFailure()
{
    // TODO: show error messages.
    m_passwordInput->setEnabled(true);
    m_passwordInput->setAlert(true);
    m_passwordInput->clear();
    m_passwordInput->setFocus();

    QPoint globalStart = mapToGlobal(m_passwordInput->pos());
    m_tooltip->show(globalStart.x() + m_passwordInput->width() / 2,
                    globalStart.y() + m_passwordInput->height());
}

void AuthDialog::setupUI()
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setMaximumWidth(380);

    addButtons(QStringList() << tr("Cancel") << tr("Confirm"));
    setOnButtonClickedClose(false);
    setDefaultButton(1);

    connect(this, &AuthDialog::buttonClicked, [this](int index, QString) {
        switch (index) {
            case 0:
                emit rejected();
                break;
            case 1:
                emit okClicked();
                break;
            default:;
        }
    });

    connect(m_passwordInput, &DPasswordEdit::textChanged, [this] {
        if (m_passwordInput->text().length() == 0) return;

        m_tooltip->hide();
        m_passwordInput->setAlert(false);
    });

    QPixmap icon;
    if (!m_iconName.isEmpty() && QIcon::hasThemeIcon(m_iconName)) {
        icon = QIcon::fromTheme(m_iconName).pixmap(48, 48);
    } else {
        icon = QPixmap(":/images/default.svg");
    }
    setIconPixmap(icon);

    m_adminsCombo->setFixedHeight(24);
    m_adminsCombo->hide();
    m_passwordInput->setFixedHeight(24);
    m_passwordInput->setFocus();
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

void AuthDetails::openUrl(const QString& url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void AuthDetails::openAction(const QString &url)
{
    // FIXME what's this? :)
    QProcess::startDetached("polkit-kde-authorization", QStringList() << url);
}


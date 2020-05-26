#include "accessibledefine.h"

#include <QDebug>

#include "AuthDialog.h"
#include "errortooltip.h"

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QToolButton>
#include <QFrame>
#include <QMenu>

#include <DVerticalLine>
#include <DWindowMinButton>
#include <DWindowCloseButton>
#include <DWindowMaxButton>
#include <DIconButton>
#include <DPasswordEdit>
#include <DLabel>
#include <DSuggestButton>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

SET_WIDGET_ACCESSIBLE(AuthDialog, QAccessible::Dialog,"authdialog")
SET_WIDGET_ACCESSIBLE(ErrorTooltip,QAccessible::StaticText, "errortip")
SET_WIDGET_ACCESSIBLE(QWidget, QAccessible::Form, "widget")
SET_LABEL_ACCESSIBLE(QLabel, "label")
SET_WIDGET_ACCESSIBLE(QComboBox, QAccessible::ComboBox, "combobox")
SET_WIDGET_ACCESSIBLE(QPushButton, QAccessible::Button, "btn")
SET_WIDGET_ACCESSIBLE(QLineEdit, QAccessible::EditableText, "lineedit")
SET_WIDGET_ACCESSIBLE(QCheckBox, QAccessible::CheckBox, "checkbox")
SET_BUTTON_ACCESSIBLE(QToolButton, "toolbutton")
SET_WIDGET_ACCESSIBLE(QFrame, QAccessible::Form, "frame")
SET_WIDGET_ACCESSIBLE(QMenu, QAccessible::PopupMenu, "menu")

SET_WIDGET_ACCESSIBLE(DVerticalLine, QAccessible::Form, "VerticalLine")
SET_BUTTON_ACCESSIBLE(DWindowMinButton, "WindowMinButton")
SET_BUTTON_ACCESSIBLE(DWindowCloseButton, "WindowCloseButton")
SET_BUTTON_ACCESSIBLE(DWindowMaxButton, "WindowMaxButton")
SET_BUTTON_ACCESSIBLE(DIconButton, "IconButton")
SET_WIDGET_ACCESSIBLE(DPasswordEdit, QAccessible::EditableText, "PasswordEdit")
SET_LABEL_ACCESSIBLE(DLabel, "imagebutton")
SET_BUTTON_ACCESSIBLE(DSuggestButton, "showpassword")
SET_WIDGET_ACCESSIBLE(DLineEdit, QAccessible::EditableText, "LineEdit")

QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
    USE_ACCESSIBLE(classname, AuthDialog);
    USE_ACCESSIBLE(classname, ErrorTooltip);
    USE_ACCESSIBLE(classname, QWidget);
    USE_ACCESSIBLE(classname, QLabel);
    USE_ACCESSIBLE(classname, QComboBox);
    USE_ACCESSIBLE(classname, QPushButton);
    USE_ACCESSIBLE(classname, QLineEdit);
    USE_ACCESSIBLE(classname, QCheckBox);
    USE_ACCESSIBLE(classname, QToolButton);
    USE_ACCESSIBLE(classname, QFrame);
    USE_ACCESSIBLE(classname, QMenu);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DVerticalLine);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DWindowMinButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DWindowCloseButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DWindowMaxButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DIconButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DPasswordEdit);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DLabel);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DSuggestButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DLineEdit);

    return interface;
}

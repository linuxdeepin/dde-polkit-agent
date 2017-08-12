TARGET = dde-polkit-agent
TEMPLATE = app
QT = core gui widgets dbus
CONFIG += link_pkgconfig c++11
PKGCONFIG += dtkwidget polkit-qt5-1 dframeworkdbus

DBUS_ADAPTORS += com.deepin.Polkit1AuthAgent.xml

SOURCES += policykitlistener.cpp \
    AuthDialog.cpp \
    main.cpp \
    errortooltip.cpp \
    usersmanager.cpp

HEADERS += policykitlistener.h \
    AuthDialog.h \
    errortooltip.h \
    usersmanager.h

RESOURCES += images.qrc

DISTFILES += com.deepin.Polkit1AuthAgent.xml \
    polkit-dde-authentication-agent-1.desktop

TRANSLATIONS = translations/dde-polkit-agent.ts

# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    system($$PWD/translate_generation.sh)
}

target.path = /usr/lib/polkit-1-dde

desktop.path = /etc/xdg/autostart/
desktop.files += polkit-dde-authentication-agent-1.desktop

qm_files.path = /usr/share/dde-polkit-agent/translations/
qm_files.files = translations/*.qm

INSTALLS += target desktop qm_files

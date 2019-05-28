TARGET = dde-polkit-agent
TEMPLATE = app
QT = core gui widgets dbus
CONFIG += link_pkgconfig c++11
PKGCONFIG += dtkcore dtkwidget polkit-qt5-1 dframeworkdbus

load(dtk_qmake)

DBUS_ADAPTORS += com.deepin.Polkit1AuthAgent.xml

SOURCES += policykitlistener.cpp \
    AuthDialog.cpp \
    main.cpp \
    errortooltip.cpp \
    usersmanager.cpp \
    pluginmanager.cpp

HEADERS += policykitlistener.h \
    AuthDialog.h \
    errortooltip.h \
    usersmanager.h \
    pluginmanager.h \
    agent-extension.h \
    agent-extension-proxy.h

include(libdde-auth/libdde-auth.pri)

RESOURCES += images.qrc

DISTFILES += com.deepin.Polkit1AuthAgent.xml

TRANSLATIONS = translations/dde-polkit-agent.ts

# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    system($$PWD/translate_generation.sh)
}

deepin {
    DEFINES += ENABLE_DEEPIN_AUTH
}

target.path = /usr/lib/polkit-1-dde

qm_files.path = /usr/share/dde-polkit-agent/translations/
qm_files.files = translations/*.qm

headers.path = /usr/include/dpa
headers.files = agent-extension-proxy.h agent-extension.h

INSTALLS += target qm_files headers

TARGET = dde-polkit-agent
TEMPLATE = app
QT += core gui widgets dbus concurrent
CONFIG += link_pkgconfig c++11
PKGCONFIG += polkit-qt5-1 dframeworkdbus dtkwidget

load(dtk_qmake)

DBUS_ADAPTORS += com.deepin.Polkit1AuthAgent.xml

SOURCES += policykitlistener.cpp \
    AuthDialog.cpp \
    main.cpp \
    usersmanager.cpp \
    pluginmanager.cpp

HEADERS += policykitlistener.h \
    AuthDialog.h \
    usersmanager.h \
    pluginmanager.h \
    agent-extension.h \
    agent-extension-proxy.h \
    accessibledefine.h \
    accessible.h

RESOURCES += images.qrc

DISTFILES += com.deepin.Polkit1AuthAgent.xml

TRANSLATIONS = translations/dde-polkit-agent.ts

# 增加安全编译参数
QMAKE_LFLAGS += -z noexecstack -pie -z relro -z now
QMAKE_CFLAGS += -fstack-protector-all -D_FORTITY_SOURCE=1
QMAKE_CXXFLAGS += -fstack-protector-all -D_FORTITY_SOURCE=1

# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    system($$PWD/translate_generation.sh)
}

target.path = /usr/lib/polkit-1-dde

qm_files.path = /usr/share/dde-polkit-agent/translations/
qm_files.files = translations/*.qm

headers.path = /usr/include/dpa
headers.files = agent-extension-proxy.h agent-extension.h

INSTALLS += target qm_files headers

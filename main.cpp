/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
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

#include <DApplication>

#include <PolkitQt1/Subject>

#include "policykitlistener.h"

#include <QDebug>

DWIDGET_USE_NAMESPACE

#define APP_NAME "dde-polkit-agent"
#define APP_DISPLAY_NAME "Deepin Polkit Agent"
#define AUTH_DBUS_PATH "/com/deepin/polkit/AuthenticationAgent"

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setApplicationName(APP_NAME);
    a.setApplicationDisplayName(APP_DISPLAY_NAME);
    a.setApplicationVersion("0.1");
    a.setQuitOnLastWindowClosed(false);


    if (!a.setSingleInstance(APP_NAME, DApplication::UserScope)) {
        qWarning() << "polkit is running!";
        return 0;
    }

    PolkitQt1::UnixSessionSubject session(getpid());
    PolicyKitListener listener;

    if (!listener.registerListener(session, AUTH_DBUS_PATH)) {
        qWarning() << "register listener failed!";
        return -1;
    }

    a.setTheme("light");
    a.loadTranslator();

    return a.exec();
}

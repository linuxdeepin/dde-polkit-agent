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

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setApplicationName("dde-polkit-agent");
    a.setApplicationDisplayName("Deepin Polkit Agent");
    a.setApplicationVersion("0.1");
    a.setQuitOnLastWindowClosed(false);

    a.setTheme("light");
    a.loadTranslator();

    PolkitQt1::UnixSessionSubject session(getpid());

    PolicyKitListener listener;
    listener.registerListener(session, "/com/deepin/polkit/AuthenticationAgent");

    return a.exec();
}

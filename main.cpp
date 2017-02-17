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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QMap>

#include "agent-extension.h"
#include "agent-extension-proxy.h"

using namespace dpa;

class PluginManager : public QObject, AgentExtensionProxy
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);

    void reduce(const QString &actionID, const QString &username, const QString passwd);

    const QString & actionID() const Q_DECL_OVERRIDE { return m_actionID; }
    const QString & username() const Q_DECL_OVERRIDE { return m_username; }
    const QString & password() const Q_DECL_OVERRIDE { return m_password; }

private:
    QList<AgentExtension*> m_plugins;
    QString m_actionID;
    QString m_username;
    QString m_password;

    void load();
    AgentExtension *loadFile(const QString &file);
};

#endif // PLUGINMANAGER_H

#include "pluginmanager.h"

#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QJsonObject>
#include <QJsonArray>

static const QString FieldDescription = "description";
static const QString FieldFilters = "filters";
static const QString FieldExec = "exec";

static const QString FilterAction = "action_id";

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
    load();
}

void PluginManager::load()
{

    QDir dir("/usr/lib/polkit-1-dde/plugins/");
    QFileInfoList pluginFiles = dir.entryInfoList((QStringList("*.so")));

    for (const QFileInfo &pluginFile : pluginFiles) {
       AgentExtension *plugin = loadFile(pluginFile.absoluteFilePath());
       if (plugin)
           m_plugins << plugin;
    }
}

AgentExtension *PluginManager::loadFile(const QString &filePath)
{
    qDebug() << "try to load plugin: " << filePath;

    if (!QLibrary::isLibrary(filePath))
        return nullptr;

    QPluginLoader *loader = new QPluginLoader(filePath, this);

    const QJsonObject meta = loader->metaData().value("MetaData").toObject();
    const QString apiVersion = meta.value("api_version").toString();

    if (apiVersion == "1.0") {
        AgentExtension *ret = qobject_cast<AgentExtension*>(loader->instance());

        if (ret) ret->initialize(this);

        return ret;
    } else {
        qWarning() << "failed to load plugin file: " << loader->errorString();
        loader->unload();
        loader->deleteLater();
    }

    return nullptr;

}

void PluginManager::reduce(const QString &actionID, const QString &username, const QString passwd)
{
    m_actionID = actionID;
    m_username = username;
    m_password = passwd;

    for (AgentExtension *plugin : m_plugins) {
        if (plugin->interestedActions().contains(actionID) || plugin->interestedActions().isEmpty()) {
            plugin->extendedDo();
        }
    }

    m_actionID = "";
    m_username = "";
    m_password = "";
}

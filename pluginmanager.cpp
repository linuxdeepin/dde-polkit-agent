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

QList<QButtonGroup*> PluginManager::reduceGetOptions(const QString &actionID)
{
    QList<QButtonGroup*> ret;

    for (AgentExtension *plugin : m_plugins) {
        if (plugin->interestedActions().contains(actionID) || plugin->interestedActions().isEmpty()) {
            ret << plugin->options();
        }
    }

    return ret;
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

        qDebug() << "done loading plugin: " << filePath;

        return ret;
    } else {
        qWarning() << "failed to load plugin file: " << loader->errorString();
        loader->unload();
        loader->deleteLater();
    }

    return nullptr;

}

void PluginManager::reduce(const QString &username, const QString passwd)
{
    m_username = username;
    m_password = passwd;

    for (AgentExtension *plugin : m_plugins) {
        if (plugin->interestedActions().contains(m_actionID) || plugin->interestedActions().isEmpty()) {
            plugin->extendedDo();
        }
    }

    m_username = "";
    m_password = "";
}

void PluginManager::setActionID(const QString &actionID)
{
    m_actionID = actionID;
}

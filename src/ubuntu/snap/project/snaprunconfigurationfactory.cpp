#include "snaprunconfigurationfactory.h"
#include "snaprunconfiguration.h"
#include "snapcraftproject.h"

#include <ubuntu/ubuntuconstants.h>
#include <projectexplorer/target.h>

#include <utils/qtcassert.h>

namespace Ubuntu {
namespace Internal {

SnapRunConfigurationFactory::SnapRunConfigurationFactory()
{
    setObjectName(QStringLiteral("SnapRunConfigurationFactory"));
}

QList<Core::Id> SnapRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent,
                                                                  ProjectExplorer::IRunConfigurationFactory::CreationMode mode) const
{
    Q_UNUSED(mode);

    if (!parent)
        return {};
    if (!qobject_cast<SnapcraftProject *>(parent->project()))
        return {};

    return {
        Constants::SNAP_RUNCONFIGURATION_ID
    };
}

QString SnapRunConfigurationFactory::displayNameForId(Core::Id id) const
{
    if (id == Constants::SNAP_RUNCONFIGURATION_ID)
        return tr("Snapcraft Runconfiguration");

    return QString();
}

bool SnapRunConfigurationFactory::canCreate(ProjectExplorer::Target *parent, Core::Id id) const
{
    return availableCreationIds(parent, ProjectExplorer::IRunConfigurationFactory::AutoCreate).contains(id);
}

bool SnapRunConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    return availableCreationIds(parent, ProjectExplorer::IRunConfigurationFactory::AutoCreate).contains(ProjectExplorer::idFromMap(map));
}

bool SnapRunConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const
{
    return availableCreationIds(parent, ProjectExplorer::IRunConfigurationFactory::AutoCreate).contains(product->id());
}

ProjectExplorer::RunConfiguration *SnapRunConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product)
{
    QTC_ASSERT(canClone(parent, product), return nullptr);
    return new SnapRunConfiguration(parent, static_cast<SnapRunConfiguration *>(product));
}

ProjectExplorer::RunConfiguration *SnapRunConfigurationFactory::doCreate(ProjectExplorer::Target *parent, Core::Id id)
{
    QTC_ASSERT(canCreate(parent, id), return nullptr);
    if (id == Constants::SNAP_RUNCONFIGURATION_ID)
        return new SnapRunConfiguration(parent);
    return nullptr;
}

ProjectExplorer::RunConfiguration *SnapRunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    QTC_ASSERT(canRestore(parent, map), return nullptr);

    SnapRunConfiguration *conf = new SnapRunConfiguration(parent);
    if (conf->fromMap(map))
        return conf;

    //something went wrong
    delete conf;
    return nullptr;

}

} // namespace Internal
} // namespace Ubuntu

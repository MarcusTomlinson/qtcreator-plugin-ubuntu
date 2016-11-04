#include "snapcraftbuildconfiguration.h"

#include <ubuntu/ubuntuconstants.h>
#include <QDebug>

namespace Ubuntu {
namespace Internal {

SnapcraftBuildConfiguration::SnapcraftBuildConfiguration(ProjectExplorer::Target *target)
    : ProjectExplorer::BuildConfiguration(target, Constants::SNAPCRAFT_BUILDCONFIGURATION_ID)
{

}

SnapcraftBuildConfiguration::SnapcraftBuildConfiguration(ProjectExplorer::Target *target, ProjectExplorer::BuildConfiguration *source)
    : ProjectExplorer::BuildConfiguration(target, source)
{

}

bool SnapcraftBuildConfiguration::fromMap(const QVariantMap &map)
{
    return ProjectExplorer::BuildConfiguration::fromMap(map);
}

QVariantMap SnapcraftBuildConfiguration::toMap() const
{
    QVariantMap data = ProjectExplorer::BuildConfiguration::toMap();
    qDebug()<<"Storing ID: "<<ProjectExplorer::idFromMap(data);
    return data;
}

ProjectExplorer::NamedWidget *SnapcraftBuildConfiguration::createConfigWidget()
{
    return nullptr;
}

ProjectExplorer::BuildConfiguration::BuildType SnapcraftBuildConfiguration::buildType() const
{
    return ProjectExplorer::BuildConfiguration::Unknown;
}

} // namespace Internal
} // namespace Ubuntu

#include "snapcraftbuildconfiguration.h"

#include <ubuntu/ubuntuconstants.h>

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
    return ProjectExplorer::BuildConfiguration::toMap();
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

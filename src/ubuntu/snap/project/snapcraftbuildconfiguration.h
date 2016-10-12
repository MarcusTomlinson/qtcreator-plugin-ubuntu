#ifndef UBUNTU_INTERNAL_SNAPCRAFTBUILDCONFIGURATION_H
#define UBUNTU_INTERNAL_SNAPCRAFTBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftBuildConfigurationFactory;

class SnapcraftBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
    Q_OBJECT

public:
    friend class SnapcraftBuildConfigurationFactory;
    // ProjectConfiguration interface
    virtual bool fromMap(const QVariantMap &map) override;
    virtual QVariantMap toMap() const override;

    // BuildConfiguration interface
    virtual ProjectExplorer::NamedWidget *createConfigWidget() override;
    virtual BuildType buildType() const override;

protected:
    SnapcraftBuildConfiguration(ProjectExplorer::Target *target);
    SnapcraftBuildConfiguration(ProjectExplorer::Target *target, BuildConfiguration *source);
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTBUILDCONFIGURATION_H

#ifndef UBUNTU_INTERNAL_SNAPCRAFTBUILDCONFIGURATIONFACTORY_H
#define UBUNTU_INTERNAL_SNAPCRAFTBUILDCONFIGURATIONFACTORY_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftBuildConfigurationFactory;

class SnapcraftBuildInfo : public ProjectExplorer::BuildInfo
{
public:
    SnapcraftBuildInfo(const SnapcraftBuildConfigurationFactory *factory);
};

class SnapcraftBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
public:
    SnapcraftBuildConfigurationFactory();

    // IBuildConfigurationFactory interface
    virtual QList<ProjectExplorer::BuildInfo *> availableBuilds(const ProjectExplorer::Target *parent) const override;

    virtual int priority(const ProjectExplorer::Target *parent) const override;
    virtual int priority(const ProjectExplorer::Kit *k, const QString &projectPath) const override;

    virtual QList<ProjectExplorer::BuildInfo *> availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const override;
    virtual ProjectExplorer::BuildConfiguration *create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const override;
    virtual bool canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const override;
    virtual ProjectExplorer::BuildConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map) override;
    virtual bool canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const override;
    virtual ProjectExplorer::BuildConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) override;

private:
    ProjectExplorer::BuildInfo *createBuildInfo (const ProjectExplorer::Kit *k, const QString &projectPath) const;
    Utils::FileName shadowBuildDirectory(const ProjectExplorer::Kit *k, const QString &projectPath) const;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTBUILDCONFIGURATIONFACTORY_H

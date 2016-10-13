#ifndef UBUNTU_INTERNAL_SNAPCRAFTBUILDSTEPFACTORY_H
#define UBUNTU_INTERNAL_SNAPCRAFTBUILDSTEPFACTORY_H

#include <projectexplorer/buildstep.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftBuildStepFactory : public ProjectExplorer::IBuildStepFactory
{
    Q_OBJECT
public:
    SnapcraftBuildStepFactory(QObject *parent = nullptr);

    // IBuildStepFactory interface
public:
    virtual QList<ProjectExplorer::BuildStepInfo> availableSteps(ProjectExplorer::BuildStepList *parent) const override;
    virtual ProjectExplorer::BuildStep *create(ProjectExplorer::BuildStepList *parent, Core::Id id) override;
    virtual ProjectExplorer::BuildStep *clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) override;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTBUILDSTEPFACTORY_H

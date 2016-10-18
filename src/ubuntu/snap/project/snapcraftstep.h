#ifndef UBUNTU_INTERNAL_SNAPCRAFTSTEP_H
#define UBUNTU_INTERNAL_SNAPCRAFTSTEP_H

#include <projectexplorer/abstractprocessstep.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftBuildStepFactory;
class SnapcraftBuildConfigurationFactory;

class SnapcraftStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    friend class SnapcraftBuildStepFactory;
    friend class SnapcraftBuildConfigurationFactory;

    // BuildStep interface
    virtual bool init(QList<const ProjectExplorer::BuildStep *> &earlierSteps) override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;

    Utils::FileName snapcraftCommand () const;

protected:
    SnapcraftStep(ProjectExplorer::BuildStepList *bsl);
    SnapcraftStep(ProjectExplorer::BuildStepList *bsl, SnapcraftStep *bs);
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTSTEP_H

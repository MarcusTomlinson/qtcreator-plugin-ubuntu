#ifndef UBUNTU_INTERNAL_SNAPCRAFTRSYNCSTEP_H
#define UBUNTU_INTERNAL_SNAPCRAFTRSYNCSTEP_H

#include <projectexplorer/abstractprocessstep.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftBuildStepFactory;
class SnapcraftBuildConfigurationFactory;

class SnapcraftRsyncStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    friend class SnapcraftBuildStepFactory;
    friend class SnapcraftBuildConfigurationFactory;

    // BuildStep interface
    virtual bool init(QList<const ProjectExplorer::BuildStep *> &earlierSteps) override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;

protected:
    SnapcraftRsyncStep(ProjectExplorer::BuildStepList *bsl);
    SnapcraftRsyncStep(ProjectExplorer::BuildStepList *bsl, SnapcraftRsyncStep *bs);
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTRSYNCSTEP_H

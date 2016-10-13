#include "snapcraftbuildstepfactory.h"
#include "snapcraftrsyncstep.h"
#include "snapcraftstep.h"
#include "snapcraftproject.h"

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/target.h>
#include <utils/algorithm.h>

namespace Ubuntu {
namespace Internal {

SnapcraftBuildStepFactory::SnapcraftBuildStepFactory(QObject *parent) : ProjectExplorer::IBuildStepFactory (parent)
{

}

QList<ProjectExplorer::BuildStepInfo> SnapcraftBuildStepFactory::availableSteps(ProjectExplorer::BuildStepList *parent) const
{
    if (parent->id() != ProjectExplorer::Constants::BUILDSTEPS_BUILD)
        return {};

    if (!parent->target() || !qobject_cast<SnapcraftProject *>(parent->target()->project()))
        return {};

    return {
        ProjectExplorer::BuildStepInfo(Constants::SNAPCRAFT_RSYNCBUILSSTEP_ID, tr("Prepare build")),
        ProjectExplorer::BuildStepInfo(Constants::SNAPCRAFT_BUILDSTEP_ID, tr("Snapcraft"))
    };
}

ProjectExplorer::BuildStep *SnapcraftBuildStepFactory::create(ProjectExplorer::BuildStepList *parent, Core::Id id)
{
    if (!Utils::contains(availableSteps(parent), [&id](const ProjectExplorer::BuildStepInfo &step){return id == step.id;}))
        return nullptr;

    if (id == Constants::SNAPCRAFT_RSYNCBUILSSTEP_ID) {
        return new SnapcraftRsyncStep(parent);
    } else if (id == Constants::SNAPCRAFT_BUILDSTEP_ID) {
        return new SnapcraftStep(parent);
    }

    return nullptr;
}

ProjectExplorer::BuildStep *SnapcraftBuildStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product)
{
    Core::Id id = product->id();
    if (!Utils::contains(availableSteps(parent), [&id](const ProjectExplorer::BuildStepInfo &step){return id == step.id;}))
        return nullptr;

    if (id == Constants::SNAPCRAFT_RSYNCBUILSSTEP_ID) {
        return new SnapcraftRsyncStep(parent, static_cast<SnapcraftRsyncStep *>(product));
    } else if (id == Constants::SNAPCRAFT_BUILDSTEP_ID) {
        return new SnapcraftStep(parent, static_cast<SnapcraftStep *>(product));
    }

    return nullptr;
}

} // namespace Internal
} // namespace Ubuntu

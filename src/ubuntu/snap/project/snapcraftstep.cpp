#include "snapcraftstep.h"
#include "snapcraftproject.h"
#include "snapcraftbuildconfiguration.h"

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/target.h>

namespace Ubuntu {
namespace Internal {

SnapcraftStep::SnapcraftStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, Constants::SNAPCRAFT_BUILDSTEP_ID)
{
    setDefaultDisplayName(tr("Snapcraft"));
}

SnapcraftStep::SnapcraftStep(ProjectExplorer::BuildStepList *bsl, SnapcraftStep *bs)
    : ProjectExplorer::AbstractProcessStep(bsl, bs)
{

}

bool SnapcraftStep::init(QList<const ProjectExplorer::BuildStep *> &)
{
    //QString projectDir = target()->project()->projectDirectory().toString();
    ProjectExplorer::BuildConfiguration *bc = target()->activeBuildConfiguration();
    if(!bc)
        return false;

    //TODO take snapcraft binary from Kit
    //TODO force snapcraft to use the qmake from the Kit by setting up the env correctly

    Utils::FileName snapcraftBin = Utils::FileName::fromString(Constants::UBUNTU_SCRIPTPATH);
    snapcraftBin = snapcraftBin.appendPath(QString::fromLatin1("run_snapcraft.py"));

    ProjectExplorer::ProcessParameters *param = processParameters();
    param->setWorkingDirectory(bc->buildDirectory().toUserOutput());
    param->setCommand(snapcraftBin.toUserOutput());
    param->setMacroExpander(bc->macroExpander());
    param->setEnvironment(bc->environment());

    return true;
}

ProjectExplorer::BuildStepConfigWidget *SnapcraftStep::createConfigWidget()
{
    return new ProjectExplorer::SimpleBuildStepConfigWidget(this);
}

} // namespace Internal
} // namespace Ubuntu

#include "snapcraftstep.h"
#include "snapcraftproject.h"
#include "snapcraftbuildconfiguration.h"

#include <ubuntu/snap/settings/snapcraftkitinformation.h>

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/target.h>
#include <utils/environment.h>

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

    Utils::FileName snapcraftBin = Utils::FileName::fromString(Constants::UBUNTU_SCRIPTPATH);
    snapcraftBin = snapcraftBin.appendPath(QString::fromLatin1("run_snapcraft.py"));

    ProjectExplorer::ProcessParameters *param = processParameters();
    param->setWorkingDirectory(bc->buildDirectory().toUserOutput());
    param->setCommand(snapcraftBin.toUserOutput());
    param->setArguments(QString::fromLatin1("-s '%1'").arg(snapcraftBin.toFileInfo().absoluteFilePath()));
    param->setMacroExpander(bc->macroExpander());
    param->setEnvironment(bc->environment());

    return true;
}

ProjectExplorer::BuildStepConfigWidget *SnapcraftStep::createConfigWidget()
{
    return new ProjectExplorer::SimpleBuildStepConfigWidget(this);
}

Utils::FileName SnapcraftStep::snapcraftCommand() const
{
    Utils::Environment env = Utils::Environment::systemEnvironment();
    Utils::FileName fallback = env.searchInPath(QStringLiteral("snapcraft"));
    if (!target())
        return fallback;

    Utils::FileName bin = SnapcraftKitInformation::snapcraftPath(target()->kit());
    if (!bin.exists())
        return fallback;

    return bin;
}

} // namespace Internal
} // namespace Ubuntu

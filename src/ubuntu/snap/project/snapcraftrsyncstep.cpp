#include "snapcraftrsyncstep.h"
#include "snapcraftproject.h"
#include "snapcraftbuildconfiguration.h"

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/target.h>

namespace Ubuntu {
namespace Internal {

SnapcraftRsyncStep::SnapcraftRsyncStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, Constants::SNAPCRAFT_RSYNCBUILSSTEP_ID)
{
    setDefaultDisplayName(tr("Prepare build"));
}

SnapcraftRsyncStep::SnapcraftRsyncStep(ProjectExplorer::BuildStepList *bsl, SnapcraftRsyncStep *bs)
    : ProjectExplorer::AbstractProcessStep(bsl, bs)
{

}

bool SnapcraftRsyncStep::init(QList<const ProjectExplorer::BuildStep *> &)
{
    QString projectDir = target()->project()->projectDirectory().toString();

    ProjectExplorer::BuildConfiguration *bc = target()->activeBuildConfiguration();
    if(!bc)
        return false;

    ProjectExplorer::ProcessParameters *param = processParameters();
    param->setWorkingDirectory(bc->buildDirectory().toUserOutput());
    param->setCommand(QStringLiteral("rsync"));
    param->setArguments(QString::fromLatin1("-a --delete \"%1/\" \"./\"")
                        .arg(QDir::cleanPath(projectDir)));
    param->setMacroExpander(bc->macroExpander());
    param->setEnvironment(bc->environment());

    return true;
}

ProjectExplorer::BuildStepConfigWidget *SnapcraftRsyncStep::createConfigWidget()
{
    return new ProjectExplorer::SimpleBuildStepConfigWidget(this);
}



} // namespace Internal
} // namespace Ubuntu

#include "snapcraftstep.h"
#include "snapcraftproject.h"
#include "snapcraftbuildconfiguration.h"

#include <ubuntu/snap/settings/snapcraftkitinformation.h>
#include <ubuntu/ubuntuconstants.h>
#include <ubuntu/clicktoolchain.h>

#include <projectexplorer/target.h>
#include <utils/environment.h>

#include <QRegularExpression>


namespace Ubuntu {
namespace Internal {

static const char * PACKAGE_NAME_REGEX = "^Snapped ([\\S]+\\.snap)$";

SnapcraftStep::SnapcraftStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl, Constants::SNAPCRAFT_BUILDSTEP_ID)
{
    setDefaultDisplayName(tr("Snapcraft"));

    connect(qobject_cast<SnapcraftProject *>(target()->project()), &SnapcraftProject::snapVersionChanged,
            this, &SnapcraftStep::packagePathChanged);
    connect(qobject_cast<SnapcraftProject *>(target()->project()), &SnapcraftProject::displayNameChanged,
            this, &SnapcraftStep::packagePathChanged);
}

SnapcraftStep::SnapcraftStep(ProjectExplorer::BuildStepList *bsl, SnapcraftStep *bs)
    : ProjectExplorer::AbstractProcessStep(bsl, bs)
{
    connect(qobject_cast<SnapcraftProject *>(target()->project()), &SnapcraftProject::snapVersionChanged,
            this, &SnapcraftStep::packagePathChanged);
    connect(qobject_cast<SnapcraftProject *>(target()->project()), &SnapcraftProject::displayNameChanged,
            this, &SnapcraftStep::packagePathChanged);
}

void SnapcraftStep::stdOutput(const QString &line)
{
    m_lastLine = line;
    ProjectExplorer::AbstractProcessStep::stdOutput(line);
}

void SnapcraftStep::processStarted()
{
    m_packagePath.clear();
    emit packagePathChanged();

    ProjectExplorer::AbstractProcessStep::processStarted();
}

void SnapcraftStep::processFinished(int exitCode, QProcess::ExitStatus status)
{
    Utils::FileName newPackagePath;

    ProjectExplorer::AbstractProcessStep::processFinished(exitCode, status);
    if (status == QProcess::NormalExit && exitCode == 0) {
        QRegularExpression exp((QLatin1String(PACKAGE_NAME_REGEX)));
        QRegularExpressionMatch m = exp.match(m_lastLine);
        if(m.hasMatch()) {
            ProjectExplorer::BuildConfiguration *bc = target()->activeBuildConfiguration();
            if(bc)
                newPackagePath = bc->buildDirectory().appendPath(m.captured(1));
        }
    }

    if (m_packagePath != newPackagePath) {
        m_packagePath = newPackagePath;
        emit packagePathChanged();
    }
}

bool SnapcraftStep::processSucceeded(int exitCode, QProcess::ExitStatus status)
{
    if (!ProjectExplorer::AbstractProcessStep::processSucceeded(exitCode, status))
        return false;

    return m_packagePath.toFileInfo().exists();
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

Utils::FileName SnapcraftStep::packagePath() const
{
    if (!m_packagePath.isEmpty())
        return m_packagePath;

    ProjectExplorer::BuildConfiguration *bc = target()->activeBuildConfiguration();
    SnapcraftProject *pro = qobject_cast<SnapcraftProject *>(target()->project());
    ProjectExplorer::ToolChain *tc = ProjectExplorer::ToolChainKitInformation::toolChain(target()->kit());
    if(bc && pro && tc && !pro->displayName().isEmpty() && !pro->snapVersion().isEmpty())
        return bc->buildDirectory().appendPath(QStringLiteral("%1_%2_%3.snap")
                                                         .arg(pro->displayName())
                                                         .arg(pro->snapVersion())
                                                         .arg(ClickToolChain::abiToArchitectureName(tc->targetAbi())));
    return Utils::FileName();
}

} // namespace Internal
} // namespace Ubuntu

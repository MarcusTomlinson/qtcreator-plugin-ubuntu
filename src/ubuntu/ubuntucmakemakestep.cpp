/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 2.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Benjamin Zeller <benjamin.zeller@canonical.com>
 */
#include "ubuntucmakemakestep.h"
#include "ubuntuconstants.h"
#include "ubuntuprojectguesser.h"
#include "clicktoolchain.h"

#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/buildstep.h>
#include <projectexplorer/projectconfiguration.h>
#include <projectexplorer/deployconfiguration.h>
#include <projectexplorer/ioutputparser.h>
#include <projectexplorer/customparser.h>
#include <cmakeprojectmanager/cmakeproject.h>
#include <cmakeprojectmanager/cmaketoolmanager.h>
#include <cmakeprojectmanager/cmakeprojectconstants.h>
#include <utils/qtcassert.h>

namespace Ubuntu {
namespace Internal {

/*!
 * \class UbuntuCMakeMakeStepFactory
 * Factory class to create UbuntuCMakeMakeStep
 * build steps
 */
QList<Core::Id> UbuntuCMakeMakeStepFactory::availableCreationIds(ProjectExplorer::BuildStepList *parent) const
{
    if(!canHandle(parent->target()))
        return QList<Core::Id>();

    if (parent->id() == ProjectExplorer::Constants::BUILDSTEPS_DEPLOY)
        return QList<Core::Id>() << Core::Id(Constants::UBUNTU_DEPLOY_MAKESTEP_ID);

    return QList<Core::Id>() << Core::Id(Constants::UBUNTU_CLICK_CMAKE_MAKESTEP_ID);
}

/*!
 * \brief UbuntuCMakeBuildConfigurationFactory::canHandle
 * checks if we can create buildconfigurations for the given target
 */
bool UbuntuCMakeMakeStepFactory::canHandle(const ProjectExplorer::Target *t) const
{
    QTC_ASSERT(t, return false);
    if (!t->project()->supportsKit(t->kit()))
        return false;

    if(ProjectExplorer::DeviceKitInformation::deviceId(t->kit()) == ProjectExplorer::Constants::DESKTOP_DEVICE_ID) {
        return UbuntuProjectGuesser::isClickAppProject(t->project());
    }

    ProjectExplorer::ToolChain* tc = ProjectExplorer::ToolChainKitInformation::toolChain(t->kit());
    if(!tc || tc->type() != QLatin1String(Constants::UBUNTU_CLICK_TOOLCHAIN_ID))
        return false;

    return t->project()->id() == Core::Id(CMakeProjectManager::Constants::CMAKEPROJECT_ID);
}

QString UbuntuCMakeMakeStepFactory::displayNameForId(const Core::Id id) const
{
    if (id == Constants::UBUNTU_CLICK_CMAKE_MAKESTEP_ID)
        return tr("UbuntuSDK-Make", "Display name for UbuntuCMakeMakeStep id.");
    if (id == Constants::UBUNTU_DEPLOY_MAKESTEP_ID)
        return tr("UbuntuSDK create deployment package", "Display name for UbuntuCMakeDeployStep id.");
    return QString();
}

bool UbuntuCMakeMakeStepFactory::canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const
{
    if (canHandle(parent->target()))
        return availableCreationIds(parent).contains(id);
    return false;
}

ProjectExplorer::BuildStep *UbuntuCMakeMakeStepFactory::create(ProjectExplorer::BuildStepList *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;

   if ( id == Core::Id(Constants::UBUNTU_DEPLOY_MAKESTEP_ID) ) {
        UbuntuCMakeDeployStep *step = new UbuntuCMakeDeployStep(parent);
        step->setUseNinja(false);
        step->setClean(false);
        return step;
    }

    UbuntuCMakeMakeStep *step = new UbuntuCMakeMakeStep(parent);
    if (parent->id() == ProjectExplorer::Constants::BUILDSTEPS_CLEAN) {
        step->setUseNinja(false);
        step->setClean(true);
        step->setAdditionalArguments(QLatin1String("clean"));
    }
    return step;
}

bool UbuntuCMakeMakeStepFactory::canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::BuildStep *UbuntuCMakeMakeStepFactory::restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;

    ProjectExplorer::BuildStep* step = create(parent,ProjectExplorer::idFromMap(map));
    if(step->fromMap(map))
        return step;

    delete step;
    return 0;
}

bool UbuntuCMakeMakeStepFactory::canClone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) const
{
    return canCreate(parent, product->id());
}

ProjectExplorer::BuildStep *UbuntuCMakeMakeStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product)
{
    if (!canClone(parent, product))
        return 0;

    if(product->id() == Core::Id(Constants::UBUNTU_DEPLOY_MAKESTEP_ID))
        return new UbuntuCMakeDeployStep(parent,static_cast<UbuntuCMakeDeployStep *>(product));
    else if(product->id() == Core::Id(Constants::UBUNTU_DEPLOY_MAKESTEP_ID))
        return new UbuntuCMakeMakeStep(parent, static_cast<UbuntuCMakeMakeStep *>(product));

    QTC_ASSERT(false,return 0);
}

/*!
 * \class UbuntuCMakeMakeStep
 * Represents a make or make clean call in the Ubuntu-SDK build chain
 */
UbuntuCMakeMakeStep::UbuntuCMakeMakeStep(ProjectExplorer::BuildStepList *bsl)
    : MakeStep(bsl,Core::Id(Constants::UBUNTU_CLICK_CMAKE_MAKESTEP_ID))
{
    setDefaultDisplayName(tr("Ubuntu SDK Make"));
}

UbuntuCMakeMakeStep::UbuntuCMakeMakeStep(ProjectExplorer::BuildStepList *bsl, UbuntuCMakeMakeStep *bs)
    : MakeStep(bsl,bs)
{
}

UbuntuCMakeMakeStep::~UbuntuCMakeMakeStep()
{

}

QString UbuntuCMakeMakeStep::makeCommand(ProjectExplorer::ToolChain *tc, const Utils::Environment &env) const
{
    if (tc)
        return tc->makeCommand(env);
    return QString::fromLatin1(Constants::UBUNTU_CLICK_MAKE_WRAPPER).arg(Constants::UBUNTU_SCRIPTPATH);
}

UbuntuCMakeDeployStep::UbuntuCMakeDeployStep(ProjectExplorer::BuildStepList *bsl)
    : MakeStep(bsl,Core::Id(Constants::UBUNTU_DEPLOY_MAKESTEP_ID))
{
    setDefaultDisplayName(tr("UbuntuSDK create deploy package"));
    setAdditionalArguments(QString::fromLatin1("DESTDIR=%1 install").arg(QLatin1String(Constants::UBUNTU_DEPLOY_DESTDIR)));
}

UbuntuCMakeDeployStep::UbuntuCMakeDeployStep(ProjectExplorer::BuildStepList *bsl, UbuntuCMakeDeployStep *bs)
    : MakeStep(bsl,bs)
{

}

ProjectExplorer::BuildStepConfigWidget *UbuntuCMakeDeployStep::createConfigWidget()
{
    return new ProjectExplorer::SimpleBuildStepConfigWidget(this);
}

UbuntuCMakeDeployStep::~UbuntuCMakeDeployStep()
{

}

bool UbuntuCMakeDeployStep::fromMap(const QVariantMap &map)
{
    if(MakeStep::fromMap(map)) {
        setAdditionalArguments(QString::fromLatin1("DESTDIR=%1 install").arg(QLatin1String(Constants::UBUNTU_DEPLOY_DESTDIR)));
        return true;
    }
    return false;
}

UbuntuClickPackageStep::UbuntuClickPackageStep(ProjectExplorer::BuildStepList *bsl)
    : ProjectExplorer::AbstractProcessStep(bsl,Constants::UBUNTU_CLICK_PACKAGESTEP_ID)
{

}

UbuntuClickPackageStep::UbuntuClickPackageStep(ProjectExplorer::BuildStepList *bsl, UbuntuClickPackageStep *bs)
    : ProjectExplorer::AbstractProcessStep(bsl,bs)
{
}

UbuntuClickPackageStep::~UbuntuClickPackageStep()
{

}


void UbuntuClickPackageStep::stdOutput(const QString &line)
{
    m_lastLine = line;
    AbstractProcessStep::stdOutput(line);
}

void UbuntuClickPackageStep::stdError(const QString &line)
{
    AbstractProcessStep::stdError(line);
}

void UbuntuClickPackageStep::processFinished(int exitCode, QProcess::ExitStatus status)
{
    if( exitCode == 0 && status == QProcess::NormalExit ) {
        QRegularExpression exp(QLatin1String(Constants::UBUNTU_CLICK_SUCCESS_PACKAGE_REGEX));
        QRegularExpressionMatch m = exp.match(m_lastLine);
        if(m.hasMatch())
            m_clickPackageName = m.captured(1);
    }

    ProjectExplorer::AbstractProcessStep::processFinished(exitCode,status);
}

bool UbuntuClickPackageStep::init()
{
    m_tasks.clear();

    ProjectExplorer::BuildConfiguration *bc = buildConfiguration();
    if (!bc){
        ProjectExplorer::Task t(ProjectExplorer::Task::Error
                                ,tr("No valid BuildConfiguration set for step: %1").arg(displayName())
                                ,Utils::FileName(),-1
                                ,ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
        m_tasks.append(t);

        //UbuntuClickPackageStep::run will stop if tasks exist
        return true;

    }


    //builds the process arguments
    QStringList arguments;
    arguments << QLatin1String("build")
              << bc->buildDirectory().toString()
                 + QDir::separator()
                 + QString::fromLatin1(Constants::UBUNTU_DEPLOY_DESTDIR);

    setIgnoreReturnValue(false);

    ProjectExplorer::ProcessParameters* params = processParameters();
    params->setMacroExpander(bc->macroExpander());

    //setup process parameters
    params->setWorkingDirectory(bc->buildDirectory().toString());
    params->setCommand(QLatin1String("click"));
    params->setArguments(Utils::QtcProcess::joinArgs(arguments));

    Utils::Environment env = bc->environment();
    // Force output to english for the parsers. Do this here and not in the toolchain's
    // addToEnvironment() to not screw up the users run environment.
    env.set(QLatin1String("LC_ALL"), QLatin1String("C"));
    params->setEnvironment(env);

    params->resolveAll();

    ProjectExplorer::IOutputParser *parser = target()->kit()->createOutputParser();
    if (parser) {
        setOutputParser(parser);
        outputParser()->setWorkingDirectory(params->effectiveWorkingDirectory());
    }
    return AbstractProcessStep::init();
}

void UbuntuClickPackageStep::run(QFutureInterface<bool> &fi)
{
    if (m_tasks.size()) {
       foreach (const ProjectExplorer::Task& task, m_tasks) {
           addTask(task);
       }
       emit addOutput(tr("Configuration is invalid. Aborting build")
                      ,ProjectExplorer::BuildStep::MessageOutput);
       fi.reportResult(false);
       emit finished();
       return;
    }

    AbstractProcessStep::run(fi);
}

ProjectExplorer::BuildStepConfigWidget *UbuntuClickPackageStep::createConfigWidget()
{
    return new ProjectExplorer::SimpleBuildStepConfigWidget(this);
}

QString UbuntuClickPackageStep::packagePath() const
{
    if(m_clickPackageName.isEmpty())
        return QString();
    return buildConfiguration()->buildDirectory().toString()
            + QDir::separator()
            + m_clickPackageName;
}

} // namespace Internal
} // namespace Ubuntu
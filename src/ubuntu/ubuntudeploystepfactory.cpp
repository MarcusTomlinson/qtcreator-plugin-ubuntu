#include "ubuntudeploystepfactory.h"
#include "ubuntuprojectguesser.h"
#include "ubuntuconstants.h"
#include "ubuntudirectuploadstep.h"
#include "ubuntupackagestep.h"

#include <utils/qtcassert.h>

#include <projectexplorer/buildstep.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/projectexplorerconstants.h>

#include <qmlprojectmanager/qmlprojectconstants.h>

#include <cmakeprojectmanager/cmakeprojectconstants.h>

namespace Ubuntu {
namespace Internal {

QList<Core::Id> UbuntuDeployStepFactory::availableCreationIds(ProjectExplorer::BuildStepList *parent) const
{
    QList<Core::Id> types;

    if (parent->id() != ProjectExplorer::Constants::BUILDSTEPS_DEPLOY)
        return types;

    Core::Id targetDevice = ProjectExplorer::DeviceTypeKitInformation::deviceTypeId(parent->target()->kit());
    if(targetDevice != ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE && targetDevice != Ubuntu::Constants::UBUNTU_DEVICE_TYPE_ID)
        return types;

    bool isRemote = targetDevice == Ubuntu::Constants::UBUNTU_DEVICE_TYPE_ID;
    bool isCMake  = parent->target()->project()->id() == CMakeProjectManager::Constants::CMAKEPROJECT_ID;
    bool isHTML   = parent->target()->project()->id() == Ubuntu::Constants::UBUNTUPROJECT_ID;
    bool isQML    = parent->target()->project()->id() == "QmlProjectManager.QmlProject";

    if (isRemote) {
        //IF we have a remote device we just support a ubuntu toolchain
        ProjectExplorer::ToolChain *tc = ProjectExplorer::ToolChainKitInformation::toolChain(parent->target()->kit());
        if(tc && tc->type() != QLatin1String(Ubuntu::Constants::UBUNTU_CLICK_TOOLCHAIN_ID))
            return types;
    }

    if(isRemote && isCMake && !UbuntuProjectGuesser::isScopesProject(parent->target()->project()))
        types << Constants::UBUNTU_DEPLOY_UPLOADSTEP_ID
              << Constants::UBUNTU_CLICK_PACKAGESTEP_ID;
    else if(isRemote && ( isHTML || isQML ) )
        types << Constants::UBUNTU_DEPLOY_UPLOADSTEP_ID
              << Constants::UBUNTU_CLICK_PACKAGESTEP_ID;

    return types;
}

QString UbuntuDeployStepFactory::displayNameForId(const Core::Id id) const
{
    if (id == Constants::UBUNTU_DEPLOY_UPLOADSTEP_ID)
        return UbuntuDirectUploadStep::displayName();
    else if (id == Constants::UBUNTU_CLICK_PACKAGESTEP_ID)
        return tr("UbuntuSDK create click package", "Display name for UbuntuPackageStep id.");
    return QString();
}

bool UbuntuDeployStepFactory::canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const
{
    return availableCreationIds(parent).contains(id);
}

ProjectExplorer::BuildStep *UbuntuDeployStepFactory::create(ProjectExplorer::BuildStepList *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;

    if(id == Constants::UBUNTU_DEPLOY_UPLOADSTEP_ID)
        return new UbuntuDirectUploadStep(parent);
    else if (id == Constants::UBUNTU_CLICK_PACKAGESTEP_ID) {
        UbuntuPackageStep *step = new UbuntuPackageStep(parent);
        return step;
    }

    return 0;
}

bool UbuntuDeployStepFactory::canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const
{
    Core::Id toRestore = ProjectExplorer::idFromMap(map);

    //backwards compatibility to older projects
    if( toRestore == Constants::UBUNTU_DEPLOY_MAKESTEP_ID )
        return canCreate(parent,Core::Id(Constants::UBUNTU_CLICK_PACKAGESTEP_ID));

    return canCreate(parent,toRestore);
}

ProjectExplorer::BuildStep *UbuntuDeployStepFactory::restore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map)
{
    Core::Id id = ProjectExplorer::idFromMap(map);
    if(!canCreate(parent,id))
        return 0;

    //backwards compatibility to older projects
    if( id == Constants::UBUNTU_DEPLOY_MAKESTEP_ID ) {
        UbuntuPackageStep *step = new UbuntuPackageStep(parent);
        return step;
    }

    ProjectExplorer::BuildStep* step = create(parent,id);
    if (!step->fromMap(map)) {
        delete step;
        return 0;
    }

    return step;
}

bool UbuntuDeployStepFactory::canClone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product) const
{
    return canCreate(parent,product->id());
}

ProjectExplorer::BuildStep *UbuntuDeployStepFactory::clone(ProjectExplorer::BuildStepList *parent, ProjectExplorer::BuildStep *product)
{
    if (!canClone(parent, product))
        return 0;

    const Core::Id id = product->id();
    if(id == Constants::UBUNTU_DEPLOY_UPLOADSTEP_ID)
        return new UbuntuDirectUploadStep(parent, static_cast<UbuntuDirectUploadStep *>(product));
    else if(id == Core::Id(Constants::UBUNTU_CLICK_PACKAGESTEP_ID))
        return new UbuntuPackageStep(parent, static_cast<UbuntuPackageStep *>(product));

    return 0;
}

} // namespace Internal
} // namespace Ubuntu

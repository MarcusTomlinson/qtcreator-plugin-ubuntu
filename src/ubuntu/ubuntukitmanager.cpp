#include "ubuntukitmanager.h"
#include "clicktoolchain.h"
#include "ubuntuconstants.h"
#include "ubuntucmaketool.h"
#include "ubuntudevice.h"
#include "ubuntuclickdialog.h"
#include "ubuntuqtversion.h"

#include <projectexplorer/kitmanager.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/toolchainmanager.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/devicesupport/devicemanager.h>
#include <debugger/debuggeritemmanager.h>
#include <debugger/debuggerkitinformation.h>
#include <qtsupport/qtkitinformation.h>
#include <cmakeprojectmanager/cmakekitinformation.h>
#include <cmakeprojectmanager/cmaketoolmanager.h>
#include <qtsupport/qtversionmanager.h>

#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

namespace Ubuntu {
namespace Internal {

enum {
    debug = 0
};

static bool equalKits(ProjectExplorer::Kit *a, ProjectExplorer::Kit *b)
{
    return ProjectExplorer::ToolChainKitInformation::toolChain(a) == ProjectExplorer::ToolChainKitInformation::ToolChainKitInformation::toolChain(b);
}

static bool lessThanToolchain (const ClickToolChain* left, const ClickToolChain* right)
{
    const UbuntuClickTool::Target &leftTarget = left->clickTarget();
    const UbuntuClickTool::Target &rightTarget = right->clickTarget();

    if(leftTarget.majorVersion < rightTarget.majorVersion)
        return true;
    if(leftTarget.minorVersion < rightTarget.minorVersion)
        return true;

    return false;
}

UbuntuKitManager::UbuntuKitManager()
{
}

QList<ClickToolChain *> UbuntuKitManager::clickToolChains()
{
    QList<ClickToolChain *> toolchains;
    // having a empty toolchains list will remove all autodetected kits for android
    // exactly what we want in that case
    foreach (ProjectExplorer::ToolChain *tc, ProjectExplorer::ToolChainManager::toolChains()) {
        if(tc) {
            if (!tc->isAutoDetected())
                continue;
            if (tc->type() != QLatin1String(Constants::UBUNTU_CLICK_TOOLCHAIN_ID))
                continue;
            toolchains << static_cast<ClickToolChain *>(tc);
        }
    }
    return toolchains;
}

UbuntuQtVersion *UbuntuKitManager::createOrFindQtVersion(ClickToolChain *tc)
{

    QString qmakePath = QStringLiteral("%1/.config/ubuntu-sdk/qmake-%2-%3")
            .arg(QDir::homePath())
            .arg(tc->clickTarget().framework)
            .arg(tc->clickTarget().architecture);

    if(!QFile::exists(qmakePath)) {
        QFile qmakeTemplate(Constants::UBUNTU_RESOURCE_PATH+QStringLiteral("/ubuntu/scripts/qtc_chroot_qmake"));
        if(qmakeTemplate.open(QIODevice::ReadOnly)) {

            QTextStream in(&qmakeTemplate);
            QString templ = in.readAll();

            templ = templ
                    .arg(tc->clickTarget().architecture)
                    .arg(tc->clickTarget().framework);


            QFile qmakeScript(qmakePath);

            qmakeScript.open(QIODevice::WriteOnly);
            qmakeScript.setPermissions(QFile::ExeUser | QFile::ReadUser | QFile::ReadGroup);

            {
                QTextStream out(&qmakeScript);
                out << templ;
            }

            qmakeScript.close();
        }
    } else {
        //try to find a already existing Qt Version for this chroot
        foreach (QtSupport::BaseQtVersion *qtVersion, QtSupport::QtVersionManager::versions()) {
            if (qtVersion->type() != QLatin1String(Constants::UBUNTU_QTVERSION_TYPE))
                continue;
            if (qtVersion->qmakeCommand().toFileInfo() == QFileInfo(qmakePath))
                return static_cast<UbuntuQtVersion*> (qtVersion);
        }
    }

    UbuntuQtVersion *qtVersion = new UbuntuQtVersion(Utils::FileName::fromString(qmakePath),false);
    QtSupport::QtVersionManager::addVersion(qtVersion);
    return qtVersion;
}

void UbuntuKitManager::autoCreateKit(UbuntuDevice::Ptr device)
{
    ProjectExplorer::Abi requiredAbi = ClickToolChain::architectureNameToAbi(device->architecture());
    if(requiredAbi.isNull()) {
        QMessageBox::warning(Core::ICore::mainWindow(),
                             tr("Unknown device architecture"),
                             tr("Kit autocreation for %1 is not supported!")
                             .arg(device->architecture()));
        return;
    }

    QList<ClickToolChain*> toolchains = clickToolChains();

    auto findCompatibleTc = [&](){
        ClickToolChain* match = 0;
        if(toolchains.size() > 0) {
            qSort(toolchains.begin(),toolchains.end(),lessThanToolchain);

            for( int i = toolchains.size() -1; i >= 0; i-- ) {
                ClickToolChain* tc = toolchains[i];
                if( tc->targetAbi() == requiredAbi ) {
                    match = tc;
                    break;
                }

                //the abi is compatible but not exactly the same
                //lets continue and see if we find a better candidate
                if(tc->targetAbi().isCompatibleWith(requiredAbi))
                    match = tc;
            }
        }
        return match;
    };

    //search a tk with a compatible arch
    ClickToolChain* match = findCompatibleTc();
    while(!match) {
        //create target
        int choice = QMessageBox::question(Core::ICore::mainWindow(),
                              tr("No target available"),
                              tr("There is no compatible chroot available on your system, do you want to create it now?"));

        if(choice == QMessageBox::Yes) {
            if(!UbuntuClickDialog::createClickChrootModal(false, device->architecture()))
                return;

            toolchains = clickToolChains();
            match = findCompatibleTc();
        } else
            return;
    }

    ProjectExplorer::Kit* newKit = createKit(match);
    if(newKit) {
        fixKit(newKit);

        newKit->setDisplayName(tr("%1 (GCC %2-%3-%4)")
                            .arg(device->displayName())
                            .arg(match->clickTarget().architecture)
                            .arg(match->clickTarget().framework)
                            .arg(match->clickTarget().series));

        ProjectExplorer::DeviceKitInformation::setDevice(newKit,device);
        ProjectExplorer::KitManager::registerKit(newKit);
    }
}

void UbuntuKitManager::autoDetectKits()
{
    // having a empty toolchains list will remove all autodetected kits for android
    // exactly what we want in that case
    QList<ClickToolChain *> toolchains = clickToolChains();

    QList<ProjectExplorer::Kit *> existingKits;
    foreach (ProjectExplorer::Kit *k, ProjectExplorer::KitManager::kits()) {
        if (k->isSdkProvided())
            continue;

        ProjectExplorer::ToolChain *tc = ProjectExplorer::ToolChainKitInformation::toolChain(k);
        if (tc && tc->type() != QLatin1String(Constants::UBUNTU_CLICK_TOOLCHAIN_ID))
            continue;

        CMakeProjectManager::ICMakeTool* icmake = CMakeProjectManager::CMakeKitInformation::cmakeTool(k);
        UbuntuCMakeTool* cmake = qobject_cast<UbuntuCMakeTool*>(icmake);
        //if the kit has no valid UbuntuCMakeTool, let it be removed from the code later
        if(cmake) {
            Utils::Environment env = Utils::Environment::systemEnvironment();
            k->addToEnvironment(env);
            cmake->setEnvironment(env);
        }

        //@TODO check for ubuntu device information
        if(debug) qDebug()<<"Found possible Ubuntu Kit: "<<k->displayName();
        existingKits << k;
    }

#if 0
    QMap<Abi::Architecture, QList<QtSupport::BaseQtVersion *> > qtVersionsForArch;
    foreach (QtSupport::BaseQtVersion *qtVersion, QtSupport::QtVersionManager::versions()) {
        if (qtVersion->type() != QLatin1String(Constants::ANDROIDQT))
            continue;
        QList<Abi> qtAbis = qtVersion->qtAbis();
        if (qtAbis.empty())
            continue;
        qtVersionsForArch[qtAbis.first().architecture()].append(qtVersion);
    }

    DeviceManager *dm = DeviceManager::instance();
    IDevice::ConstPtr device = dm->find(Core::Id(Constants::ANDROID_DEVICE_ID));
    if (device.isNull()) {
        // no device, means no sdk path
        foreach (Kit *k, existingKits)
            KitManager::deregisterKit(k);
        return;
    }
#endif

    // create new kits
    QList<ProjectExplorer::Kit *> newKits;
    foreach (ClickToolChain *tc, toolchains) {
        ProjectExplorer::Kit* newKit = createKit(tc);
        newKit->makeSticky();
        newKits << newKit;
    }

    //remove already existing kits
    for (int i = existingKits.count() - 1; i >= 0; --i) {
        ProjectExplorer::Kit *existingKit = existingKits.at(i);
        for (int j = 0; j < newKits.count(); ++j) {
            ProjectExplorer::Kit *newKit = newKits.at(j);
            if (equalKits(existingKit, newKit)) {
                // Kit is already registered, nothing to do
                ProjectExplorer::Kit *oldKit = existingKits.takeAt(i);
                oldKit->makeSticky();

                //make sure kit has all required informations
                fixKit(oldKit);

                newKits.removeAt(j);
                ProjectExplorer::KitManager::deleteKit(newKit);
                j = newKits.count();
            }
        }
    }

    //all kits remaining need to be removed if they don't have all informations
    foreach (ProjectExplorer::Kit *k, existingKits) {
        ProjectExplorer::ToolChain *tc = ProjectExplorer::ToolChainKitInformation::toolChain(k);
        CMakeProjectManager::ICMakeTool* icmake = CMakeProjectManager::CMakeKitInformation::cmakeTool(k);
        if (tc && tc->type() == QLatin1String(Constants::UBUNTU_CLICK_TOOLCHAIN_ID)
                && icmake && icmake->id().toString().startsWith(QLatin1String(Constants::UBUNTU_CLICK_CMAKE_TOOL_ID))
                && icmake->isValid()) {
            fixKit(k);

            //existing targets are not autodetected anymore
            k->makeUnSticky();
            k->setAutoDetected(false);
        } else {
            //has not all informations, go away
            ProjectExplorer::KitManager::deregisterKit(k);
        }
    }

    foreach (ProjectExplorer::Kit *kit, newKits) {
        ClickToolChain *tc = static_cast<ClickToolChain *>(ProjectExplorer::ToolChainKitInformation::toolChain(kit));
        //AndroidQtVersion *qt = static_cast<AndroidQtVersion *>(QtSupport::QtKitInformation::qtVersion(kit));
        kit->setDisplayName(tr("UbuntuSDK for %1 (GCC %2-%3)")
                            .arg(tc->clickTarget().architecture)
                            .arg(tc->clickTarget().framework)
                            .arg(tc->clickTarget().series));
        ProjectExplorer::KitManager::registerKit(kit);
        fixKit(kit);
    }
}

/*!
 * \brief UbuntuKitManager::createKit
 * Creates a new Kit for the Ubunut toolchain and sets default
 * values
 */
ProjectExplorer::Kit *UbuntuKitManager::createKit(ClickToolChain *tc)
{
    //@TODO find a qt version
    ProjectExplorer::Kit* newKit = new ProjectExplorer::Kit;
    newKit->setAutoDetected(false); //let the user delete that stuff
    newKit->setIconPath(Utils::FileName::fromString(QLatin1String(Constants::UBUNTU_MODE_WEB_ICON)));
    ProjectExplorer::ToolChainKitInformation::setToolChain(newKit, tc);

    //every kit gets its own instance of CMakeTool
    UbuntuCMakeTool* cmake = new UbuntuCMakeTool;
    Utils::Environment env = Utils::Environment::systemEnvironment();
    newKit->addToEnvironment(env);
    cmake->setEnvironment(env);

    CMakeProjectManager::CMakeToolManager::registerCMakeTool(cmake);
    CMakeProjectManager::CMakeKitInformation::setCMakeTool(newKit,cmake->id());


    ProjectExplorer::SysRootKitInformation::setSysRoot(newKit,Utils::FileName::fromString(UbuntuClickTool::targetBasePath(tc->clickTarget())));

    //we always want a ubuntu device
    Core::Id devTypeId = Core::Id(Constants::UBUNTU_DEVICE_TYPE_ID).withSuffix(tc->clickTarget().architecture);
    ProjectExplorer::DeviceTypeKitInformation::setDeviceTypeId(newKit,devTypeId);

    //@TODO add gdbserver support
    QtSupport::QtKitInformation::setQtVersion(newKit, createOrFindQtVersion(tc));
    return newKit;
}

/*!
 * \brief UbuntuKitManager::createOrFindDebugger
 * Tries to find a already existing ubuntu debugger, if it can not find one
 * it is registered and returned
 */
QVariant UbuntuKitManager::createOrFindDebugger(const Utils::FileName &path)
{
    if(path.isEmpty())
        return QVariant();

    QList<Debugger::DebuggerItem> debuggers = Debugger::DebuggerItemManager::debuggers();
    foreach(const Debugger::DebuggerItem& debugger,debuggers) {
        if(debugger.command() == path) {
            return debugger.id();
        }
    }


    Debugger::DebuggerItem debugger;
    debugger.setCommand(path);
    debugger.setEngineType(Debugger::GdbEngineType);
    debugger.setDisplayName(tr("Ubuntu SDK Debugger"));
    debugger.setAutoDetected(true);
    //multiarch debugger
    ProjectExplorer::Abi abi(ProjectExplorer::Abi::UnknownArchitecture
                             ,ProjectExplorer::Abi::LinuxOS
                             ,ProjectExplorer::Abi::GenericLinuxFlavor
                             ,ProjectExplorer::Abi::UnknownFormat
                             ,0);
    debugger.setAbi(abi);
    return Debugger::DebuggerItemManager::registerDebugger(debugger);
}

/*!
 * \brief UbuntuKitManager::fixKit
 * Tries to fix a Kit if there is missing information
 */
void UbuntuKitManager::fixKit(ProjectExplorer::Kit *k)
{
    k->setAutoDetected(false);

    ClickToolChain* tc = static_cast<ClickToolChain *> (ProjectExplorer::ToolChainKitInformation::toolChain(k));
    if(!tc) {
        return;
    }

    //make sure we have the multiarch debugger
    if(!Debugger::DebuggerKitInformation::debugger(k)) {
        QVariant dId = createOrFindDebugger(tc->suggestedDebugger());
        if(dId.isValid())
            Debugger::DebuggerKitInformation::setDebugger(k,dId);
    }

    if(ProjectExplorer::SysRootKitInformation::sysRoot(k).isEmpty()) {
        ProjectExplorer::SysRootKitInformation::setSysRoot(k,Utils::FileName::fromString(UbuntuClickTool::targetBasePath(tc->clickTarget())));
    }

    //make sure we point to a ubuntu device
    Core::Id devId = ProjectExplorer::DeviceTypeKitInformation::deviceTypeId(k);
    if ( !devId.isValid() ||                                             //invalid type
         devId == Constants::UBUNTU_DEVICE_TYPE_ID ||                    //Kit uses still the old device type ids
         !devId.toString().startsWith(QLatin1String(Constants::UBUNTU_DEVICE_TYPE_ID)) //kit has a wrong device type
         ) {

        //if this kit still uses the old type ids, we try to find the correct device by name
        bool doMigration = (devId == Constants::UBUNTU_DEVICE_TYPE_ID);

        //a old kit with a incorrect device ID, lets set the correct device type id
        Core::Id devTypeId = Core::Id(Constants::UBUNTU_DEVICE_TYPE_ID).withSuffix(tc->clickTarget().architecture);
        ProjectExplorer::DeviceTypeKitInformation::setDeviceTypeId(k,devTypeId);

        if (doMigration) {
            UbuntuDevice::ConstPtr fuzzyMatch;
            UbuntuDevice::ConstPtr fullMatch;

            //lets search for a device
            ProjectExplorer::DeviceManager *devMgr = ProjectExplorer::DeviceManager::instance();
            for (int i = 0; i<devMgr->deviceCount(); i++) {
                ProjectExplorer::IDevice::ConstPtr dev = devMgr->deviceAt(i);
                if(!dev)
                    continue;

                //the type ID also checks if the architecture is correct
                if(dev->type() != devTypeId)
                    continue;

                UbuntuDevice::ConstPtr ubuntuDev = qSharedPointerCast<const UbuntuDevice>(dev);

                //we found a possible Device!
                if(!fuzzyMatch)
                    fuzzyMatch = ubuntuDev;

                //this is most likely the device that was used with this kit by using the autocreate button
                QRegularExpression regExp (QStringLiteral("^(%1\\s+\\(.*\\))$").arg(ubuntuDev->displayName()));
                QRegularExpressionMatch m = regExp.match(ubuntuDev->displayName());
                if (m.hasMatch()) {
                    fullMatch = ubuntuDev;
                    break;
                }
            }
            ProjectExplorer::DeviceKitInformation::setDevice(k,!fullMatch.isNull() ? fullMatch : fuzzyMatch);
        }
    }

    //values the user can change
    k->setSticky(ProjectExplorer::DeviceKitInformation::id(),false);
    k->setSticky(Debugger::DebuggerKitInformation::id(),false);

    //values the user cannot change
    k->setSticky(ProjectExplorer::SysRootKitInformation::id(),true);
    k->setMutable(ProjectExplorer::SysRootKitInformation::id(),false);

    //make sure we use a ubuntu Qt version
    QtSupport::QtKitInformation::setQtVersion(k, createOrFindQtVersion(tc));

}

} // namespace Internal
} // namespace Ubuntu
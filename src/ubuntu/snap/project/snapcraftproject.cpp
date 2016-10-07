/*
 * Copyright 2016 Canonical Ltd.
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

#include "snapcraftproject.h"
#include "snapcraftprojectnode.h"

#include <coreplugin/modemanager.h>
#include <coreplugin/icontext.h>
#include <coreplugin/documentmanager.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/projectmacroexpander.h>
#include <qmljs/qmljssimplereader.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtsupportconstants.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <yaml-cpp/yaml.h>
#pragma GCC diagnostic pop


using namespace Ubuntu;
using namespace Ubuntu::Internal;

SnapcraftProject::SnapcraftProject(SnapcraftProjectManager *manager, const Utils::FileName &fileName)
    : m_manager(manager),
      m_fileName(fileName) {

    setId(Constants::UBUNTUPROJECT_ID);

    setProjectContext(Core::Context(Constants::SNAPCRAFT_PROJECT_PROJECTCONTEXT));


    m_file = QPointer<SnapcraftProjectFile>(new SnapcraftProjectFile());
    Core::DocumentManager::addDocument(m_file.data(), true);
    setDocument(m_file.data());

    m_file->setFilePath(fileName);

    m_rootNode = QSharedPointer<SnapcraftProjectNode>(new SnapcraftProjectNode(this, fileName));

    ProjectExplorer::FileNode *projectFileNode = new ProjectExplorer::FileNode(fileName, ProjectExplorer::ProjectFileType, false);
    m_rootNode->addFileNodes({projectFileNode});

    QMetaObject::invokeMethod(this, "asyncUpdate", Qt::QueuedConnection);
}

QString SnapcraftProject::displayName() const {
    return m_rootNode->displayName();
}

ProjectExplorer::IProjectManager *SnapcraftProject::projectManager() const {
    return m_manager;
}

ProjectExplorer::ProjectNode *SnapcraftProject::rootProjectNode() const {
    return m_rootNode.data();
}

QStringList SnapcraftProject::files(FilesMode) const {
    QStringList files;
    //enumChild(projectDir(), files);
    return files;
}

bool SnapcraftProject::supportsKit(ProjectExplorer::Kit *, QString *) const
{
#if 0
    UbuntuKitMatcher matcher;
    if (!matcher.matches(k)) {
        if(errorMessage)
            *errorMessage = tr("Only Desktop and Ubuntu Kits are supported");
        return false;
    }
#endif
    return true;
}

bool SnapcraftProject::needsConfiguration() const
{
    return targets().size() == 0;
}

bool SnapcraftProject::requiresTargetPanel() const
{
    return true;
}

QString SnapcraftProject::shadowBuildDirectory(const QString &proFilePath
                                            , const ProjectExplorer::Kit *k
                                            , const QString &suffix
                                            , const ProjectExplorer::BuildConfiguration::BuildType buildType)
{
    if (proFilePath.isEmpty())
        return QString();

    QFileInfo info(proFilePath);

    QtSupport::BaseQtVersion *version = QtSupport::QtKitInformation::qtVersion(k);
    if (version)
        return info.absolutePath();

    const QString projectName = QFileInfo(proFilePath).completeBaseName();
    ProjectExplorer::ProjectMacroExpander expander(proFilePath, projectName, k, suffix, buildType);
    QDir projectDir = QDir(projectDirectory(Utils::FileName::fromString(proFilePath)).toString());
    QString buildPath = expander.expand(Core::DocumentManager::buildDirectory());
    return QDir::cleanPath(projectDir.absoluteFilePath(buildPath));
}

void SnapcraftProject::asyncUpdate()
{
    try {
        YAML::Node yaml = YAML::LoadFile(m_fileName.toString().toStdString());
        if (!m_rootNode->syncFromYAMLNode(yaml)) {
            qDebug()<<"ZOOOONK";
        }
    } catch (const YAML::Exception &e) {
        qDebug() << e.what();
    }

    emit displayNameChanged();
}

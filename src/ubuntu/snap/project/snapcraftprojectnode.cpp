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

#include "snapcraftprojectnode.h"

#include <projectexplorer/nodesvisitor.h>

#include <QFileInfo>
#include <QDir>
#include <QTimer>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <yaml-cpp/yaml.h>
#pragma GCC diagnostic pop

namespace Ubuntu {
namespace Internal {

SnapcraftProjectNode::SnapcraftProjectNode(SnapcraftProject *rootProject, const Utils::FileName &projectFilePath)
    : ProjectExplorer::ProjectNode (projectFilePath),
      m_rootProject(rootProject)
{
    setDisplayName(projectFilePath.parentDir().toString());
}

bool SnapcraftProjectNode::syncFromYAMLNode(YAML::Node rootNode)
{
    try {
        QString displayName = QString::fromStdString(rootNode["name"].as<std::string>());
        setDisplayName(displayName);

        YAML::Node parts = rootNode["parts"];
        if (!parts.IsMap()) {
            qDebug()<<"Parts is not a map";
            return false;
        }

        QList<ProjectExplorer::FolderNode *> existingNodes = subFolderNodes();
        QList<ProjectExplorer::FolderNode *> nodesToRemove;
        QList<ProjectExplorer::FolderNode *> nodesToAdd;

        QStringList partsFromYaml;
        QStringList existingParts;

        for (const auto &part : existingNodes) {
            existingParts << part->displayName();
        }

        //iterate over the parts
        for (auto it = parts.begin(); it != parts.end(); ++it) {

            YAML::Node subProject = it->second;
            QString partName = QString::fromStdString(it->first.as<std::string>());
            QString subType  = QString::fromStdString(subProject["plugin"].as<std::string>());
            QString source   = QDir::cleanPath(QString::fromStdString(subProject["source"].as<std::string>()));

            partsFromYaml << partName;

            Utils::FileName sourcePath = filePath().parentDir().appendPath(source);

            int idx = existingParts.indexOf(partName);
            if (idx >= 0) {
                //check if source is still the same
                SnapcraftGenericPartNode *n = static_cast<SnapcraftGenericPartNode *>(existingNodes.at(idx));
                if (n->filePath() == sourcePath)
                    continue;

                nodesToRemove << n;
            }

            SnapcraftGenericPartNode *partNode =  new SnapcraftGenericPartNode(partName, sourcePath);
            nodesToAdd << partNode;
        }

        QSet<QString> obsoleteParts = existingParts.toSet() - partsFromYaml.toSet();
        for (const auto &part : obsoleteParts) {
            int idx = existingParts.indexOf(part);
            if (idx >= 0)
                nodesToRemove << existingNodes.at(idx);
        }

        //remove old nodes
        removeFolderNodes(nodesToRemove);
        addFolderNodes(nodesToAdd);
    } catch (const YAML::Exception &e) {
        qDebug()<<"ERRROR ERROR ERROR "<<e.what();
        return false;
    }

    return true;
}

/*!
    \class Ubuntu::Internal::SnapcraftGenericPartNode
    \brief The SnapcraftGenericPartNode class
    Fallback node if the subproject type can not be handled by the plugin directly
 */

static void enumChild(const QDir &dir, QSet<Utils::FileName> &dirs, QSet<Utils::FileName> &res) {
    foreach (const QFileInfo &info, dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files|QDir::Hidden)) {
        if (info.isDir()) {
            dirs << Utils::FileName(info);
            enumChild(QDir(info.absoluteFilePath()), dirs, res);
        } else {
            res.insert(Utils::FileName(info));
        }
    }
}

SnapcraftGenericPartNode::SnapcraftGenericPartNode(const QString &partName, const Utils::FileName &folderPath)
    : ProjectExplorer::FolderNode (folderPath, ProjectExplorer::ProjectNodeType, partName)
{
    scheduleProjectScan();

    m_watcher.addPath(folderPath.toString());
    QObject::connect(&m_watcher, &QFileSystemWatcher::directoryChanged, [this](const QString &){
        scheduleProjectScan();
    });
}

void SnapcraftGenericPartNode::scheduleProjectScan()
{
    if (m_scanning)
        return;

    qDebug()<<"Scheduling Project scan";

    m_scanning = true;

    QTimer *rescanTimer = new QTimer();
    rescanTimer->setSingleShot(true);
    rescanTimer->start(0);
    QObject::connect(rescanTimer, &QTimer::timeout, [this, rescanTimer](){
        qDebug()<<"Starting Project scan";
        delete rescanTimer;
        this->scanProjectDirectory();

        m_scanning = false;
    });
}

void SnapcraftGenericPartNode::removeFileNodes(const QList<Utils::FileName> &files)
{
    foreach(const Utils::FileName &f, files) {
        if(f.toFileInfo().isDir())
            continue;

        FindFileNodesForFileVisitor vis(f);
        this->accept(&vis);

        removeFileNodes(vis.nodes());
    }
}

void SnapcraftGenericPartNode::removeFolderNodes(QList<Utils::FileName> &dirs)
{

    qSort(dirs.begin(), dirs.end(),[](const Utils::FileName &a, const Utils::FileName &b){
        return a.toFileInfo().absolutePath() < b.toFileInfo().absolutePath();
    });

    qDebug()<<"About to remove "<<dirs;

    foreach(const Utils::FileName &f, dirs) {
        if(!f.toFileInfo().isDir())
            continue;

        FindNodesForFolderVisitor vis(f);
        this->accept(&vis);

        m_watcher.removePath(f.toFileInfo().absolutePath());
        removeFolderNodes(vis.nodes());
    }
}

void SnapcraftGenericPartNode::scanProjectDirectory()
{
    QSet<Utils::FileName> dirs;
    QSet<Utils::FileName> files;
    enumChild(QDir(filePath().toString()), dirs, files);

    FindFilesAndDirsVisitor vis;
    this->accept(&vis);

    QSet<Utils::FileName> oldFiles = QSet<Utils::FileName>::fromList(vis.filePaths());
    QSet<Utils::FileName> oldDirs  = QSet<Utils::FileName>::fromList(vis.dirPaths());
    QSet<Utils::FileName> newFiles(files);
    QSet<Utils::FileName> newDirs(dirs);

    QSet<Utils::FileName> filesToRemove = oldFiles - newFiles;
    QSet<Utils::FileName> filesToAdd    = newFiles - oldFiles;
    QList<Utils::FileName> dirsToRemove  = (oldDirs - newDirs).toList();
    QSet<Utils::FileName> dirsToAdd     = newDirs - oldDirs;

    qDebug()<<"Removing dirs " <<dirsToRemove;
    qDebug()<<"Adding dirs   " <<dirsToAdd;
    qDebug()<<"Removing files" <<filesToRemove;
    qDebug()<<"Adding files  " <<filesToAdd;

    removeFileNodes(filesToRemove.toList());
    removeFolderNodes(dirsToRemove);

    for (const Utils::FileName &dir :  dirsToAdd) {
        Utils::FileName relativeName = dir.relativeChildPath(filePath());
        QStringList relativeNameList = relativeName.toUserOutput().split(QDir::separator());
        createOrFindFolder(relativeNameList);
    }

    for (const Utils::FileName &file :  filesToAdd) {
        Utils::FileName folderPath = file.parentDir();

        ProjectExplorer::FolderNode *parentNode = nullptr;
        qDebug()<<"Compare "<<folderPath<<filePath();
        if (folderPath == filePath()) {
            parentNode = this;
        } else {
            Utils::FileName relativeName = folderPath.relativeChildPath(filePath());
            QStringList relativeNameList = relativeName.toUserOutput().split(QDir::separator());
            parentNode = createOrFindFolder(relativeNameList);
        }

        ProjectExplorer::FileNode *fNode = new ProjectExplorer::FileNode(file, ProjectExplorer::UnknownFileType, false);
        parentNode->addFileNodes({fNode});
    }
}

ProjectExplorer::FolderNode *SnapcraftGenericPartNode::createOrFindFolder(const QStringList &folder)
{
    ProjectExplorer::FolderNode *currFolder = this;

    Utils::FileName currentPath = filePath();

    for (const QString &folderName: folder) {

        QList<ProjectExplorer::FolderNode *> subnodes = subFolderNodes();
        currentPath = currentPath.appendPath(folderName);

        auto check = [&folderName](ProjectExplorer::FolderNode *f) {
            return f->filePath().fileName() == folderName;
        };

        auto it = std::find_if(subnodes.begin(), subnodes.end(), check);

        if (it != subnodes.end()) {
            //node exists already
            currFolder = *it;
            continue;
        }

        //does not exist lets create a new one
        ProjectExplorer::FolderNode *fNode = new ProjectExplorer::FolderNode(currentPath, ProjectExplorer::FolderNodeType, folderName);
        currFolder->addFolderNodes({fNode});
        currFolder = fNode;
        m_watcher.addPath(currentPath.toFileInfo().absolutePath());
    }
    return currFolder;
}

Utils::FileNameList FindFilesAndDirsVisitor::filePaths() const
{
    return m_filePaths;
}

Utils::FileNameList FindFilesAndDirsVisitor::dirPaths() const
{
    return m_dirPaths;
}

void FindFilesAndDirsVisitor::visitProjectNode(ProjectExplorer::ProjectNode *projectNode)
{
    visitFolderNode(projectNode);
}

void FindFilesAndDirsVisitor::visitFolderNode(ProjectExplorer::FolderNode *folderNode)
{
    if (folderNode->nodeType() != ProjectExplorer::ProjectNodeType)
        m_dirPaths.append(folderNode->filePath());

    for (const ProjectExplorer::FileNode *fileNode : folderNode->fileNodes())
        m_filePaths.append(fileNode->filePath());
}

FindFileNodesForFileVisitor::FindFileNodesForFileVisitor(const Utils::FileName &f)
    : m_file(f)
{

}

QList<ProjectExplorer::FileNode *> FindFileNodesForFileVisitor::nodes() const
{
    return m_nodes;
}

void FindFileNodesForFileVisitor::visitProjectNode(ProjectExplorer::ProjectNode *projectNode)
{
    visitFolderNode(projectNode);
}

void FindFileNodesForFileVisitor::visitFolderNode(ProjectExplorer::FolderNode *folderNode)
{
    for(auto fileNode: folderNode->fileNodes()) {
        if (fileNode->filePath() == m_file)
            m_nodes.append(fileNode);
    }
}

FindNodesForFolderVisitor::FindNodesForFolderVisitor(const Utils::FileName &f)
    : m_folder(f)
{

}

QList<ProjectExplorer::FolderNode *> FindNodesForFolderVisitor::nodes() const
{
    return m_nodes;
}

void FindNodesForFolderVisitor::visitProjectNode(ProjectExplorer::ProjectNode *)
{
    return;
}

void FindNodesForFolderVisitor::visitFolderNode(ProjectExplorer::FolderNode *folderNode)
{
    m_nodes.append(folderNode);
}



} // namespace Internal
} // namespace Ubuntu

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

#ifndef UBUNTU_INTERNAL_SNAPCRAFTPROJECTNODE_H
#define UBUNTU_INTERNAL_SNAPCRAFTPROJECTNODE_H

#include <projectexplorer/projectnodes.h>
#include <projectexplorer/nodesvisitor.h>
#include <yaml-cpp/node/node.h>

#include <QPointer>

class QFileSystemWatcher;

namespace Ubuntu {
namespace Internal {

class FindFilesAndDirsVisitor : public ProjectExplorer::NodesVisitor {
public:
    Utils::FileNameList filePaths() const;
    Utils::FileNameList dirPaths() const;

    void visitProjectNode(ProjectExplorer::ProjectNode *projectNode) override;
    void visitFolderNode(ProjectExplorer::FolderNode *folderNode) override;

private:
    Utils::FileNameList m_filePaths;
    Utils::FileNameList m_dirPaths;
};

class FindFileNodesForFileVisitor : public ProjectExplorer::NodesVisitor {

public:
    FindFileNodesForFileVisitor (const Utils::FileName &f);
    QList<ProjectExplorer::FileNode *> nodes () const;

    void visitProjectNode(ProjectExplorer::ProjectNode *projectNode) override;
    void visitFolderNode(ProjectExplorer::FolderNode *folderNode) override;

private:
    Utils::FileName m_file;
    QList<ProjectExplorer::FileNode *> m_nodes;
};

class FindNodesForFolderVisitor : public ProjectExplorer::NodesVisitor {

public:
    FindNodesForFolderVisitor (const Utils::FileName &f);
    QList<ProjectExplorer::FolderNode *> nodes () const;

    void visitProjectNode(ProjectExplorer::ProjectNode *projectNode) override;
    void visitFolderNode(ProjectExplorer::FolderNode *folderNode) override;

private:
    Utils::FileName m_folder;
    QList<ProjectExplorer::FolderNode *> m_nodes;
};

class SnapcraftProject;
class SnapcraftGenericPartNode;

class SnapcraftProjectNode : public ProjectExplorer::ProjectNode
{
public:
    SnapcraftProjectNode(SnapcraftProject *rootProject, const Utils::FileName &projectFilePath, QFileSystemWatcher *watcher);
    ~SnapcraftProjectNode();

    bool syncFromYAMLNode(YAML::Node rootNode);

private:
    SnapcraftProject *m_rootProject = nullptr;
    QPointer<QFileSystemWatcher> m_watcher;
};

class SnapcraftGenericPartFolderNode : public ProjectExplorer::FolderNode
{
public:

    using ProjectExplorer::FolderNode::FolderNode;

    // FolderNode interface
    virtual bool addFiles(const QStringList &filePaths, QStringList *notAdded) override;
    virtual bool removeFiles(const QStringList &filePaths, QStringList *notRemoved) override;
    virtual bool deleteFiles(const QStringList &filePaths) override;
};

class SnapcraftGenericPartNode : public ProjectExplorer::FolderNode
{
public:
    SnapcraftGenericPartNode(const QString &partName, const Utils::FileName &folderPath, QFileSystemWatcher *watcher);
    ~SnapcraftGenericPartNode();

    void maybeScheduleProjectScan(const QString &changedPath);
    void scheduleProjectScan();

    using ProjectExplorer::FolderNode::removeFileNodes;
    void removeFileNodes (const QList<Utils::FileName> &files);

    using ProjectExplorer::FolderNode::removeFolderNodes;
    void removeFolderNodes (QList<Utils::FileName> &dirs);

    // Node interface
    virtual QList<ProjectExplorer::ProjectAction> supportedActions(ProjectExplorer::Node *node) const override;
    virtual bool addFiles(const QStringList &filePaths, QStringList *notAdded) override;
    virtual bool removeFiles(const QStringList &filePaths, QStringList *notRemoved) override;
    virtual bool deleteFiles(const QStringList &filePaths) override;

protected:
    ProjectExplorer::FolderNode *createOrFindFolder (const QStringList &folder);
    void scanProjectDirectory ();

private:
    bool m_scanning = false;
    QMetaObject::Connection m_watcherConnection;
    QPointer<QFileSystemWatcher> m_watcher;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTPROJECTNODE_H

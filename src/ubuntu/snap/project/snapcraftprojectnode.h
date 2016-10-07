#ifndef UBUNTU_INTERNAL_SNAPCRAFTPROJECTNODE_H
#define UBUNTU_INTERNAL_SNAPCRAFTPROJECTNODE_H

#include <projectexplorer/projectnodes.h>
#include <projectexplorer/nodesvisitor.h>
#include <yaml-cpp/node/node.h>

#include <QFileSystemWatcher>

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
    SnapcraftProjectNode(SnapcraftProject *rootProject, const Utils::FileName &projectFilePath);

    bool syncFromYAMLNode(YAML::Node rootNode);

private:
    SnapcraftProject *m_rootProject = nullptr;
};


class SnapcraftGenericPartNode : public ProjectExplorer::FolderNode
{
public:
    SnapcraftGenericPartNode(const QString &partName, const Utils::FileName &folderPath);

    void scheduleProjectScan ();

    using ProjectExplorer::FolderNode::removeFileNodes;
    void removeFileNodes (const QList<Utils::FileName> &files);

    using ProjectExplorer::FolderNode::removeFolderNodes;
    void removeFolderNodes (QList<Utils::FileName> &dirs);

protected:
    ProjectExplorer::FolderNode *createOrFindFolder (const QStringList &folder);
    void scanProjectDirectory ();

private:
    bool m_scanning = false;
    QFileSystemWatcher m_watcher;

};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTPROJECTNODE_H

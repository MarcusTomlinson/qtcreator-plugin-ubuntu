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

#ifndef SNAPCRAFTPROJECT_H
#define SNAPCRAFTPROJECT_H

#include "snapcraftprojectmanager.h"
#include "snapcraftprojectfile.h"

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/project.h>

#include <QPointer>
#include <QFileSystemWatcher>

#include <yaml-cpp/node/node.h>

namespace Ubuntu {
namespace Internal {

class UbuntuProjectManager;
class SnapcraftProjectNode;

class SnapcraftProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    SnapcraftProject(SnapcraftProjectManager *manager, const Utils::FileName &fileName);

    QString displayName() const override;
    ProjectExplorer::IProjectManager *projectManager() const override;

    ProjectExplorer::ProjectNode *rootProjectNode() const override;
    QStringList files(FilesMode fileMode) const override;

    QDir projectDir() const {
        return projectDirectory().toString();
    }

    QString filesFileName() const {
        return m_fileName.toString();
    }

    // Project interface
    bool supportsKit(ProjectExplorer::Kit *k, QString *errorMessage) const override;
    bool needsConfiguration() const override;
    bool requiresTargetPanel() const override;

    static QString shadowBuildDirectory(const QString &proFilePath, const ProjectExplorer::Kit *k,
                                        const QString &suffix = QString(),
                                        const ProjectExplorer::BuildConfiguration::BuildType buildType = ProjectExplorer::BuildConfiguration::Unknown);

protected slots:
    void asyncUpdate ();
    void maybeUpdate (const QString &pathChanged);

private:
    SnapcraftProjectManager *m_manager;
    QString m_projectName;
    QPointer<SnapcraftProjectFile> m_file;

    Utils::FileName m_fileName;
    QSharedPointer<SnapcraftProjectNode> m_rootNode;
    QFileSystemWatcher m_watcher;
};
}
}

#endif // SNAPCRAFTPROJECT_H

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

#include "snapcraftprojectmanager.h"
#include "snapcraftproject.h"

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/session.h>
#include <coreplugin/idocument.h>
#include <utils/fileutils.h>

#include <QFileInfo>


namespace Ubuntu {
namespace Internal {

SnapcraftProjectManager::SnapcraftProjectManager()
{

}

QString SnapcraftProjectManager::mimeType() const
{
    return QLatin1String(Constants::SNAPCRAFT_PROJECT_MIMETYPE);
}

ProjectExplorer::Project *SnapcraftProjectManager::openProject(const QString &fileName, QString *errorString)
{
    QFileInfo fileInfo(fileName);

    foreach (ProjectExplorer::Project *pi, ProjectExplorer::SessionManager::projects()) {
        if (fileName == pi->document()->filePath().toString()) {
            if (errorString)
                *errorString = tr("Failed opening project '%1': Project already open") .arg(QDir::toNativeSeparators(fileName));
            return 0;
        }
    }

    if (fileInfo.isFile())
        return new SnapcraftProject(this, Utils::FileName(fileInfo));

    *errorString = tr("Failed opening project '%1': Project file is not a file").arg(QDir::toNativeSeparators(fileName));
    return 0;
}

} // namespace Internal
} // namespace Ubuntu

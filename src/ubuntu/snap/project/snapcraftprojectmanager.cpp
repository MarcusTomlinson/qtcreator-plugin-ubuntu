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

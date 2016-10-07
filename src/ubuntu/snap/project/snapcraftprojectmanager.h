#ifndef UBUNTU_INTERNAL_SNAPCRAFTPROJECTMANAGER_H
#define UBUNTU_INTERNAL_SNAPCRAFTPROJECTMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftProjectManager : public ProjectExplorer::IProjectManager
{
public:
    SnapcraftProjectManager();

    // IProjectManager interface
    virtual QString mimeType() const override;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString) override;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTPROJECTMANAGER_H

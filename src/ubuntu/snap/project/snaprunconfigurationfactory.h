#ifndef UBUNTU_INTERNAL_SNAPRUNCONFIGURATIONFACTORY_H
#define UBUNTU_INTERNAL_SNAPRUNCONFIGURATIONFACTORY_H

#include <projectexplorer/runconfiguration.h>

namespace Ubuntu {
namespace Internal {

class SnapRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
public:

    SnapRunConfigurationFactory();

    // IRunConfigurationFactory interface
    virtual QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent, CreationMode mode) const override;
    virtual QString displayNameForId(Core::Id id) const override;
    virtual bool canCreate(ProjectExplorer::Target *parent, Core::Id id) const override;
    virtual bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const override;
    virtual bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const override;
    virtual ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) override;

private:
    virtual ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, Core::Id id) override;
    virtual ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent, const QVariantMap &map) override;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPRUNCONFIGURATIONFACTORY_H

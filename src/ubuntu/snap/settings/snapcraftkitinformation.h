#ifndef UBUNTU_INTERNAL_SNAPCRAFTKITINFORMATION_H
#define UBUNTU_INTERNAL_SNAPCRAFTKITINFORMATION_H

#include <projectexplorer/kitconfigwidget.h>
#include <projectexplorer/kitinformation.h>

namespace Utils {
class PathChooser;
}

namespace Ubuntu {
namespace Internal {

class SnapcraftKitInformation : public ProjectExplorer::KitInformation
{
    Q_OBJECT
public:
    SnapcraftKitInformation();

    static Utils::FileName snapcraftPath (const ProjectExplorer::Kit *k);
    static void setSnapcraftPath (ProjectExplorer::Kit *k, const Utils::FileName &snapcraft);

    // KitInformation interface
    virtual QVariant defaultValue(const ProjectExplorer::Kit *) const override;
    virtual QList<ProjectExplorer::Task> validate(const ProjectExplorer::Kit *) const override;
    virtual ItemList toUserOutput(const ProjectExplorer::Kit *) const override;
    virtual ProjectExplorer::KitConfigWidget *createConfigWidget(ProjectExplorer::Kit *) const override;

private:
    Utils::FileName m_snapcraftPath;
};

class SnapcraftKitInformationWidget : public ProjectExplorer::KitConfigWidget
{
public:
    SnapcraftKitInformationWidget(ProjectExplorer::Kit *kit, const ProjectExplorer::KitInformation *ki);
    virtual ~SnapcraftKitInformationWidget();

    // KitConfigWidget interface
    virtual QString displayName() const override;
    virtual void makeReadOnly() override;
    virtual void refresh() override;
    virtual QWidget *mainWidget() const override;
    virtual QString toolTip() const override;
    virtual QWidget *buttonWidget() const override;

private:
    void pathWasChanged();
private:
    Utils::PathChooser *m_chooser;
    bool m_ignoreChange;


};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTKITINFORMATION_H

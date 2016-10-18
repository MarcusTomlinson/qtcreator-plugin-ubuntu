#include "snapcraftkitinformation.h"

#include <projectexplorer/kit.h>

#include <utils/environment.h>
#include <utils/qtcassert.h>
#include <utils/pathchooser.h>

#include <QFileInfo>

namespace Ubuntu {
namespace Internal {

static const char TOOL_ID[] = "SnapcraftProjectManager.SnapcraftKitInformation";

SnapcraftKitInformation::SnapcraftKitInformation()
{
    setObjectName(QStringLiteral("SnapcraftKitInformation"));
    setId(TOOL_ID);
    setPriority(29000);
}

Utils::FileName SnapcraftKitInformation::snapcraftPath(const ProjectExplorer::Kit *k)
{
    return k->value(TOOL_ID, QVariant::fromValue<Utils::FileName>(Utils::FileName())).value<Utils::FileName>();
}

void SnapcraftKitInformation::setSnapcraftPath(ProjectExplorer::Kit *k, const Utils::FileName &snapcraft)
{
    if (snapcraft.toFileInfo().isExecutable()) {
        k->setValue(TOOL_ID, QVariant::fromValue<Utils::FileName>(snapcraft));
    }
}

QVariant SnapcraftKitInformation::defaultValue(const ProjectExplorer::Kit *) const
{
    Utils::Environment env = Utils::Environment::systemEnvironment();
    return QVariant::fromValue<Utils::FileName>(env.searchInPath(QStringLiteral("snapcraft")));
}

QList<ProjectExplorer::Task> SnapcraftKitInformation::validate(const ProjectExplorer::Kit *) const
{
    return QList<ProjectExplorer::Task>();
}

ProjectExplorer::KitInformation::ItemList SnapcraftKitInformation::toUserOutput(const ProjectExplorer::Kit *k) const
{
    Utils::FileName snap = snapcraftPath(k);
    return ItemList {
        qMakePair(tr("Snapcraft"), snap.exists() ? snap.toUserOutput() : tr("Unconfigured"))
    };
}

ProjectExplorer::KitConfigWidget *SnapcraftKitInformation::createConfigWidget(ProjectExplorer::Kit *kit) const
{
    return new SnapcraftKitInformationWidget(kit, this);
}

SnapcraftKitInformationWidget::SnapcraftKitInformationWidget(ProjectExplorer::Kit *kit, const ProjectExplorer::KitInformation *ki)
    : ProjectExplorer::KitConfigWidget(kit, ki)
    , m_chooser(new Utils::PathChooser)
    , m_ignoreChange(false)
{
    m_chooser->setExpectedKind(Utils::PathChooser::ExistingCommand);
    m_chooser->setFileName(SnapcraftKitInformation::snapcraftPath(kit));
    connect(m_chooser, &Utils::PathChooser::pathChanged,
            this, &SnapcraftKitInformationWidget::pathWasChanged);
}

SnapcraftKitInformationWidget::~SnapcraftKitInformationWidget()
{
    delete m_chooser;
}

QString SnapcraftKitInformationWidget::displayName() const
{
    return tr("Snapcraft");
}

void SnapcraftKitInformationWidget::makeReadOnly()
{
    m_chooser->setReadOnly(true);
}

void SnapcraftKitInformationWidget::refresh()
{
    if (!m_ignoreChange)
        m_chooser->setPath(SnapcraftKitInformation::snapcraftPath(kit()).toUserOutput());
}

QWidget *SnapcraftKitInformationWidget::mainWidget() const
{
    return m_chooser->lineEdit();
}

QString SnapcraftKitInformationWidget::toolTip() const
{
    return tr("The snapcraft binary to use for this kit.");
}

QWidget *SnapcraftKitInformationWidget::buttonWidget() const
{
    return m_chooser->buttonAtIndex(0);
}

void SnapcraftKitInformationWidget::pathWasChanged()
{
    m_ignoreChange = true;
    SnapcraftKitInformation::setSnapcraftPath(kit(), m_chooser->fileName());
    m_ignoreChange = false;
}

} // namespace Internal
} // namespace Ubuntu

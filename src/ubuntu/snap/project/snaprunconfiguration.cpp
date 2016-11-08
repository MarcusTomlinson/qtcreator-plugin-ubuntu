#include "snaprunconfiguration.h"
#include "snapcraftstep.h"
#include "snapcraftproject.h"

#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/runnables.h>
#include <projectexplorer/target.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildstep.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <utils/qtcprocess.h>
#include <utils/pathchooser.h>

#include <QFileInfo>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>

namespace Ubuntu {
namespace Internal {

const QString COMMAND_KEY = QStringLiteral("SnapProjectManager.SnapRunConfiguration.SnapCommand");
const QString DEVMODE_KEY = QStringLiteral("SnapProjectManager.SnapRunConfiguration.SnapDevMode");
const QString WORKINGDIRECTORYASPECT_ID = QStringLiteral("SnapProjectManager.SnapRunConfiguration.WorkingDirectoryAspect");
const QString ARGUMENTASPECT_ID = QStringLiteral("SnapProjectManager.SnapRunConfiguration.ArgumentAspect");
const QString TERMINALASPECT_ID = QStringLiteral("SnapProjectManager.SnapRunConfiguration.TerminalAspect");

SnapRunConfiguration::SnapRunConfiguration(ProjectExplorer::Target *parent)
    : ProjectExplorer::RunConfiguration(parent, Constants::SNAP_RUNCONFIGURATION_ID)
    , m_useDevMode(true)
    , m_workingDirectoryAspect(new ProjectExplorer::WorkingDirectoryAspect(this, WORKINGDIRECTORYASPECT_ID))
    , m_argumentAspect(new ProjectExplorer::ArgumentsAspect(this, ARGUMENTASPECT_ID))
    , m_terminalAspect(new ProjectExplorer::TerminalAspect(this, TERMINALASPECT_ID))
    , m_localEnvironmentAspect(new ProjectExplorer::LocalEnvironmentAspect(this, ProjectExplorer::LocalEnvironmentAspect::BaseEnvironmentModifier()))
{
    m_terminalAspect->setRunMode(ProjectExplorer::ApplicationLauncher::Gui);

    addExtraAspect(m_argumentAspect);
    addExtraAspect(m_terminalAspect);
    addExtraAspect(m_localEnvironmentAspect);
    addExtraAspect(m_workingDirectoryAspect);

    if(parent) {
        SnapcraftProject *pro = qobject_cast<SnapcraftProject *>(parent->project());
        if (pro) {
            setDisplayName(pro->displayName());
            QStringList comms = pro->commands();
            if (comms.size())
                m_command = comms.first();
        }
         m_workingDirectoryAspect->setDefaultWorkingDirectory(Utils::FileName::fromString(pro->projectDir().absolutePath()));
    }

    // Connect target signals
    connect(this->target(), &ProjectExplorer::Target::activeBuildConfigurationChanged,
            this, &SnapRunConfiguration::updateConfiguration);
    updateConfiguration();
}

SnapRunConfiguration::SnapRunConfiguration(ProjectExplorer::Target *parent, SnapRunConfiguration *source)
    : ProjectExplorer::RunConfiguration(parent, source),
      m_command(source->m_command)
{
    // Connect target signals
    connect(this->target(), &ProjectExplorer::Target::activeBuildConfigurationChanged,
            this, &SnapRunConfiguration::updateConfiguration);
    updateConfiguration();
}

QString SnapRunConfiguration::command() const
{
    return m_command;
}

void SnapRunConfiguration::setCommand(const QString &command)
{
    if (command != m_command) {
        m_command = command;
        emit enabledChanged();
    }
}

bool SnapRunConfiguration::fromMap(const QVariantMap &map)
{
    if (!ProjectExplorer::RunConfiguration::fromMap(map))
        return false;

    m_command = map.value(COMMAND_KEY, QString()).toString();
    m_useDevMode = map.value(DEVMODE_KEY, true).toBool();
    return true;
}

QVariantMap SnapRunConfiguration::toMap() const
{
    QVariantMap map = ProjectExplorer::RunConfiguration::toMap();
    map.insert(COMMAND_KEY, m_command);
    map.insert(DEVMODE_KEY, m_useDevMode);
    return map;
}

bool SnapRunConfiguration::isEnabled() const
{
    return !m_command.isEmpty();
}

QString SnapRunConfiguration::disabledReason() const
{
    return tr("Please select a command to run in the runconfiguration.");
}

QWidget *SnapRunConfiguration::createConfigurationWidget()
{
    return new SnapRunConfigurationWidget(this);
}

ProjectExplorer::Runnable SnapRunConfiguration::runnable() const
{
    SnapcraftStep *pckStep = activeSnapcraftStep();
    if (!pckStep) {
        return ProjectExplorer::Runnable();
    }

    Utils::FileName packagePath = pckStep->packagePath();
    if(packagePath.isEmpty()) {
        return ProjectExplorer::Runnable();
    }

    qDebug()<<"Going to run "<<packagePath.toUserOutput();

    QStringList args{
        packagePath.toUserOutput(),
        m_command
    };

    if (m_useDevMode)
        args.prepend(QStringLiteral("--devmode"));

    ProjectExplorer::StandardRunnable r;
    r.executable = Utils::FileName::fromString(Constants::UBUNTU_SCRIPTPATH).appendPath(QStringLiteral("qtc_desktop_snaprunner.py")).toString();;
    r.workingDirectory = m_workingDirectoryAspect->workingDirectory().toString();
    r.environment = m_localEnvironmentAspect->environment();
    r.commandLineArguments = Utils::QtcProcess::joinArgs(args);
    if (!m_argumentAspect->arguments().isEmpty()) {
        r.commandLineArguments.append(QStringLiteral(" -- %1").arg(m_argumentAspect->arguments()));
    }
    return r;
}

SnapcraftStep *SnapRunConfiguration::activeSnapcraftStep() const
{
    ProjectExplorer::BuildConfiguration *buildConf = activeBuildConfiguration();
    if(!buildConf)
        return nullptr;

    ProjectExplorer::BuildStepList *bsList = buildConf->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    for(ProjectExplorer::BuildStep *currStep : bsList->steps()) {
        SnapcraftStep *pckStep = qobject_cast<SnapcraftStep*>(currStep);
        if(pckStep)
            return pckStep;
    }
    return nullptr;
}

void SnapRunConfiguration::updateConfiguration()
{
    if(m_currBuildConfConn)
        disconnect(m_currBuildConfConn);

    m_currBuildConfConn = connect(activeSnapcraftStep(), &SnapcraftStep::packagePathChanged,
                                  this, &ProjectExplorer::RunConfiguration::requestRunActionsUpdate);

}

bool SnapRunConfiguration::useDevMode() const
{
    return m_useDevMode;
}

void SnapRunConfiguration::setUseDevMode(bool useDevMode)
{
    m_useDevMode = useDevMode;
}

SnapRunConfigurationWidget::SnapRunConfigurationWidget(SnapRunConfiguration *config)
    : QWidget (nullptr),
      m_rc(config),
      m_updating(false)
{
    QFormLayout *layout = new QFormLayout(this);
    layout->setMargin(0);
    layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    setLayout(layout);

    m_commandsBox = new QComboBox(this);
    updateComboBox();

    connect(m_commandsBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(commandSelected(int)));

    int idx = m_commandsBox->findText(config->command());
    if (idx >= 0)
        m_commandsBox->setCurrentIndex(idx);

    m_devmodeCheckBox = new QCheckBox(tr("Use devmode"), this);
    m_devmodeCheckBox->setChecked(config->useDevMode());
    connect(m_devmodeCheckBox, &QCheckBox::stateChanged,
            this, &SnapRunConfigurationWidget::devModeStateChanged);

    layout->addRow(tr("Command:"), m_commandsBox);
    config->extraAspect<ProjectExplorer::ArgumentsAspect>()->addToMainConfigurationWidget(this, layout);
    config->extraAspect<ProjectExplorer::WorkingDirectoryAspect>()->addToMainConfigurationWidget(this,layout);
    config->extraAspect<ProjectExplorer::TerminalAspect>()->addToMainConfigurationWidget(this,layout);
    layout->addRow(QStringLiteral(""), m_devmodeCheckBox);

    if(config->target()) {
        connect(qobject_cast<SnapcraftProject*>(config->target()->project()), &SnapcraftProject::commandListChanged,
                this, &SnapRunConfigurationWidget::updateComboBox);
    }
}

void SnapRunConfigurationWidget::updateComboBox()
{
    QString currItem = m_rc->command();

    //TODO check if project is still loading, so we do not override something we need

    m_updating = true;
    m_commandsBox->clear();
    m_commandsBox->addItem(QString());
    if(m_rc && m_rc->target()) {
        SnapcraftProject *pro = qobject_cast<SnapcraftProject *>(m_rc->target()->project());
        if (pro) {
            QStringList comms = pro->commands();
            m_commandsBox->addItems(comms);
        }
    }
    m_updating = false;

    int idx = m_commandsBox->findText(currItem);
    if (idx <= 0 && m_commandsBox->count() > 1)
        m_commandsBox->setCurrentIndex(1);
    else if (idx > 0)
        m_commandsBox->setCurrentIndex(idx);
    else
        m_commandsBox->setCurrentIndex(0);
}

void SnapRunConfigurationWidget::devModeStateChanged(int state)
{
    if (state == Qt::Checked)
        m_rc->setUseDevMode(true);
    else
        m_rc->setUseDevMode(false);
}

void SnapRunConfigurationWidget::commandSelected(const int index)
{
    if(m_updating)
        return;

    if(index == 0)
        m_rc->setCommand(QString());
    else
        m_rc->setCommand(m_commandsBox->itemText(index));
}

} // namespace Internal
} // namespace Ubuntu

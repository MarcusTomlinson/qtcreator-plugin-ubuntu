#ifndef UBUNTU_INTERNAL_SNAPRUNCONFIGURATION_H
#define UBUNTU_INTERNAL_SNAPRUNCONFIGURATION_H

#include "snaprunconfigurationfactory.h"

#include <projectexplorer/runconfiguration.h>
#include <utils/fileutils.h>

#include <QStringList>

class QComboBox;

namespace Utils {
    class PathChooser;
}

namespace ProjectExplorer {
    class WorkingDirectoryAspect;
    class ArgumentsAspect;
    class TerminalAspect;
    class LocalEnvironmentAspect;
}

namespace Ubuntu {
namespace Internal {

class SnapcraftStep;

class SnapRunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT
    friend class SnapRunConfigurationFactory;
protected:
    SnapRunConfiguration(ProjectExplorer::Target *parent);
    SnapRunConfiguration(ProjectExplorer::Target *parent, SnapRunConfiguration *source);

public:

    QString command () const;
    void setCommand (const QString &command);

    // ProjectConfiguration interface
    virtual bool fromMap(const QVariantMap &map) override;
    virtual QVariantMap toMap() const override;

    // RunConfiguration interface
    virtual bool isEnabled() const override;
    virtual QString disabledReason() const override;
    virtual QWidget *createConfigurationWidget() override;
    virtual ProjectExplorer::Runnable runnable() const override;

private:
    SnapcraftStep *activeSnapcraftStep() const;
    void updateCommandList(const QStringList &commands);
    void updateConfiguration();

private:
    QString m_command;
    QMetaObject::Connection m_currBuildConfConn;
    ProjectExplorer::WorkingDirectoryAspect* m_workingDirectoryAspect;
    ProjectExplorer::ArgumentsAspect* m_argumentAspect;
    ProjectExplorer::TerminalAspect* m_terminalAspect;
    ProjectExplorer::LocalEnvironmentAspect* m_localEnvironmentAspect;
};

class SnapRunConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    SnapRunConfigurationWidget(SnapRunConfiguration *config);

protected:
    void updateComboBox ();

protected slots:
    void commandSelected (const int index);

private:
    SnapRunConfiguration *m_rc;
    QComboBox *m_commandsBox;
    bool m_updating;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPRUNCONFIGURATION_H

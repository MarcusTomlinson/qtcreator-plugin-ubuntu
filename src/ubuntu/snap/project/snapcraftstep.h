#ifndef UBUNTU_INTERNAL_SNAPCRAFTSTEP_H
#define UBUNTU_INTERNAL_SNAPCRAFTSTEP_H

#include <projectexplorer/abstractprocessstep.h>
#include <utils/fileutils.h>

namespace Ubuntu {
namespace Internal {

class SnapcraftBuildStepFactory;
class SnapcraftBuildConfigurationFactory;

class SnapcraftStep : public ProjectExplorer::AbstractProcessStep
{
    Q_OBJECT
public:
    friend class SnapcraftBuildStepFactory;
    friend class SnapcraftBuildConfigurationFactory;

    // BuildStep interface
    virtual bool init(QList<const ProjectExplorer::BuildStep *> &earlierSteps) override;
    virtual ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;

    Utils::FileName snapcraftCommand () const;
    Utils::FileName packagePath () const;

signals:
    void packagePathChanged ();

protected:
    SnapcraftStep(ProjectExplorer::BuildStepList *bsl);
    SnapcraftStep(ProjectExplorer::BuildStepList *bsl, SnapcraftStep *bs);

    // AbstractProcessStep interface
    virtual void stdOutput(const QString &line);
    virtual void processStarted() override;
    virtual void processFinished(int exitCode, QProcess::ExitStatus status) override;
    virtual bool processSucceeded(int exitCode, QProcess::ExitStatus status) override;



private:
    QString m_lastLine;
    Utils::FileName m_packagePath;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_SNAPCRAFTSTEP_H

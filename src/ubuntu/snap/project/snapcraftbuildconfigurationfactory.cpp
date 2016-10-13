#include "snapcraftbuildconfigurationfactory.h"

#include "snapcraftproject.h"
#include "snapcraftbuildconfiguration.h"
#include "snapcraftrsyncstep.h"
#include "snapcraftstep.h"
#include <ubuntu/ubuntuconstants.h>

#include <projectexplorer/target.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/projectmacroexpander.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildsteplist.h>

#include <coreplugin/documentmanager.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>

namespace Ubuntu {
namespace Internal {

SnapcraftBuildInfo::SnapcraftBuildInfo(const SnapcraftBuildConfigurationFactory *factory)
    :ProjectExplorer::BuildInfo(factory)
{

}

SnapcraftBuildConfigurationFactory::SnapcraftBuildConfigurationFactory()
{

}

QList<ProjectExplorer::BuildInfo *> SnapcraftBuildConfigurationFactory::availableBuilds(const ProjectExplorer::Target *parent) const
{
    if (qobject_cast<SnapcraftProject *>(parent->project()))
        return {};

    QList<ProjectExplorer::BuildInfo *> infoList;
    ProjectExplorer::BuildInfo *info = createBuildInfo(parent->kit(), parent->project()->projectFilePath().toString());
    if (info)
        infoList << info;

    return infoList;
}

int SnapcraftBuildConfigurationFactory::priority(const ProjectExplorer::Target *) const
{
    return 0;
}

int SnapcraftBuildConfigurationFactory::priority(const ProjectExplorer::Kit *, const QString &) const
{
    return 0;
}

QList<ProjectExplorer::BuildInfo *> SnapcraftBuildConfigurationFactory::availableSetups(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    Utils::MimeDatabase db;

    auto mimeType = db.mimeTypeForFile(projectPath);
    if (!mimeType.matchesName(Constants::SNAPCRAFT_PROJECT_MIMETYPE))
        return {};

    QList<ProjectExplorer::BuildInfo *> infoList;
    ProjectExplorer::BuildInfo *info = createBuildInfo(k, projectPath);
    if (info)
        infoList << info;

    return infoList;
}

ProjectExplorer::BuildConfiguration *SnapcraftBuildConfigurationFactory::create(ProjectExplorer::Target *parent, const ProjectExplorer::BuildInfo *info) const
{
    SnapcraftBuildConfiguration *conf = new SnapcraftBuildConfiguration(parent);

    conf->setDisplayName(info->displayName);
    conf->setBuildDirectory(info->buildDirectory);

    ProjectExplorer::BuildStepList *bs = conf->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
    bs->insertStep(0, new SnapcraftRsyncStep(bs));
    bs->insertStep(1, new SnapcraftStep(bs));

    return conf;
}

bool SnapcraftBuildConfigurationFactory::canRestore(const ProjectExplorer::Target *, const QVariantMap &map) const
{
    return (ProjectExplorer::idFromMap(map) == Constants::SNAPCRAFT_BUILDCONFIGURATION_ID);
}

ProjectExplorer::BuildConfiguration *SnapcraftBuildConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    QTC_ASSERT(parent, return nullptr);
    SnapcraftBuildConfiguration *conf = new SnapcraftBuildConfiguration(parent);
    if (conf->fromMap(map))
        return conf;

    delete conf;
    return nullptr;
}

bool SnapcraftBuildConfigurationFactory::canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product) const
{
    QTC_ASSERT(parent, return false);
    QTC_ASSERT(product, return false);
    return (product->id() == Constants::SNAPCRAFT_BUILDCONFIGURATION_ID);
}

ProjectExplorer::BuildConfiguration *SnapcraftBuildConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *product)
{
    QTC_ASSERT(parent, return nullptr);
    QTC_ASSERT(product, return nullptr);

    SnapcraftBuildConfiguration *conf = new SnapcraftBuildConfiguration(parent, product);
    return conf;
}

ProjectExplorer::BuildInfo *SnapcraftBuildConfigurationFactory::createBuildInfo(const ProjectExplorer::Kit *k, const QString &projectPath) const
{
    SnapcraftBuildInfo *build = new SnapcraftBuildInfo(this);
    build->buildType = ProjectExplorer::BuildConfiguration::Release;
    build->typeName = tr("Build");
    build->kitId = k->id();
    build->displayName = ProjectExplorer::BuildConfiguration::buildTypeName(build->buildType);
    build->buildDirectory = shadowBuildDirectory(k, projectPath);
    return {build};
}

Utils::FileName SnapcraftBuildConfigurationFactory::shadowBuildDirectory(const ProjectExplorer::Kit *k, const QString &projectPath) const
{

    Utils::FileName projectDir = ProjectExplorer::Project::projectDirectory(Utils::FileName::fromString(projectPath));
    const QString projectName = projectDir.fileName();

    ProjectExplorer::ProjectMacroExpander expander(projectPath, projectName,
                                                   k, tr("build"),
                                                   ProjectExplorer::BuildConfiguration::Release);

    QString buildPath = expander.expand(Core::DocumentManager::buildDirectory());
    return Utils::FileName::fromString(QDir::cleanPath(QDir(projectDir.toString()).absoluteFilePath(buildPath)));
}


} // namespace Internal
} // namespace Ubuntu

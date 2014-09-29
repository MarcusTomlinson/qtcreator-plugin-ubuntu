#ifndef UBUNTU_INTERNAL_UBUNTUQTVERSION_H
#define UBUNTU_INTERNAL_UBUNTUQTVERSION_H

#include <qtsupport/baseqtversion.h>
#include <qtsupport/qtversionfactory.h>

namespace Ubuntu {
namespace Internal {

class UbuntuQtVersion : public QtSupport::BaseQtVersion
{
public:
    UbuntuQtVersion();
    UbuntuQtVersion(const Utils::FileName &path, bool isAutodetected = false, const QString &autodetectionSource = QString());
    ~UbuntuQtVersion() override;

    // BaseQtVersion interface
    virtual void fromMap(const QVariantMap &map) override;
    virtual QVariantMap toMap() const override;

    UbuntuQtVersion *clone() const override;

    QString type() const override;

    QList<ProjectExplorer::Abi> detectQtAbis() const override;

    QString description() const override;

    QString platformName() const override;
    QString platformDisplayName() const override;

    int scriptVersion() const;
    void setScriptVersion(int scriptVersion);

    static int minimalScriptVersion ();

private:
    int m_scriptVersion;
};

class UbuntuQtVersionFactory : public QtSupport::QtVersionFactory
{
public:
    // QtVersionFactory interface
    virtual bool canRestore(const QString &type) override;
    virtual QtSupport::BaseQtVersion *restore(const QString &type, const QVariantMap &data) override;
    virtual int priority() const override;
    virtual QtSupport::BaseQtVersion *create(const Utils::FileName &qmakePath, ProFileEvaluator *evaluator, bool isAutoDetected, const QString &autoDetectionSource) override;
};

} // namespace Internal
} // namespace Ubuntu

#endif // UBUNTU_INTERNAL_UBUNTUQTVERSION_H

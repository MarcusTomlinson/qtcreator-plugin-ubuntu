#ifndef DEMOSCOPE_H
#define DEMOSCOPE_H

#include <unity/scopes/ScopeBase.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/ReplyProxyFwd.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/PreviewQueryBase.h>

class %DISPLAYNAME_CAPITAL%Scope : public unity::scopes::ScopeBase
{
public:
    virtual int start(std::string const&, unity::scopes::RegistryProxy const&) override;

    virtual void stop() override;

    unity::scopes::PreviewQueryBase::UPtr preview(unity::scopes::Result const& result,
            unity::scopes::ActionMetadata const& metadata) override;

    virtual unity::scopes::SearchQueryBase::UPtr search(unity::scopes::CannedQuery const& q,
            unity::scopes::SearchMetadata const& metadata) override;
};

#endif

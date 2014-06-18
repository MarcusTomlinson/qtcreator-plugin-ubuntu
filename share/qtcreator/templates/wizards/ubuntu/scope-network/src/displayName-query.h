#ifndef DEMOQUERY_H
#define DEMOQUERY_H

#include <unity/scopes/SearchQueryBase.h>
#include <unity/scopes/ReplyProxyFwd.h>

class %DISPLAYNAME_CAPITAL%Query : public unity::scopes::SearchQueryBase
{
public:
    %DISPLAYNAME_CAPITAL%Query(unity::scopes::CannedQuery const& query, unity::scopes::SearchMetadata const& metadata);
    ~%DISPLAYNAME_CAPITAL%Query();
    virtual void cancelled() override;

    virtual void run(unity::scopes::SearchReplyProxy const& reply) override;

private:
    std::string query_;
};

#endif

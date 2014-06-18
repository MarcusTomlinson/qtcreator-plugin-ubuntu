#include "%DISPLAYNAME%-query.h"
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

using namespace unity::scopes;

%DISPLAYNAME_CAPITAL%Query::%DISPLAYNAME_CAPITAL%Query(CannedQuery const& query, SearchMetadata const& metadata) :
    SearchQueryBase(query, metadata)
{
}

%DISPLAYNAME_CAPITAL%Query::~%DISPLAYNAME_CAPITAL%Query()
{
}

void %DISPLAYNAME_CAPITAL%Query::cancelled()
{
}

void %DISPLAYNAME_CAPITAL%Query::run(unity::scopes::SearchReplyProxy const& reply)
{
    CategoryRenderer rdr;
    auto cat = reply->register_category("cat1", "Category 1", "", rdr);
    CategorisedResult res(cat);
    res.set_uri("uri");
    res.set_title("scope-A: result 1 for query \"" + query().query_string() + "\"");
    res.set_art("icon");
    res.set_dnd_uri("dnd_uri");
    reply->push(res);
}

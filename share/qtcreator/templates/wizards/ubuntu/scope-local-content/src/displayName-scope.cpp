#include "%DISPLAYNAME%-scope.h"
#include "%DISPLAYNAME%-query.h"
#include "%DISPLAYNAME%-preview.h"
#include <unity-scopes.h>

using namespace unity::scopes;

void %DISPLAYNAME_CAPITAL%Scope::start(std::string const&, RegistryProxy const&)
{
}

void %DISPLAYNAME_CAPITAL%Scope::stop()
{
}

SearchQueryBase::UPtr %DISPLAYNAME_CAPITAL%Scope::search(CannedQuery const &q, SearchMetadata const& metadata)
{
    SearchQueryBase::UPtr query(new %DISPLAYNAME_CAPITAL%Query(q, metadata));
    return query;
}

PreviewQueryBase::UPtr %DISPLAYNAME_CAPITAL%Scope::preview(Result const& result, ActionMetadata const& metadata) {
    PreviewQueryBase::UPtr preview(new %DISPLAYNAME_CAPITAL%Preview(result, metadata));
    return preview;
}

#define EXPORT __attribute__ ((visibility ("default")))

extern "C"
{

    EXPORT
    unity::scopes::ScopeBase*
    // cppcheck-suppress unusedFunction
    UNITY_SCOPE_CREATE_FUNCTION()
    {
        return new %DISPLAYNAME_CAPITAL%Scope();
    }

    EXPORT
    void
    // cppcheck-suppress unusedFunction
    UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base)
    {
        delete scope_base;
    }

}

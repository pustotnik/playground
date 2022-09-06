#pragma once

#include <string>
#include <string_view>

namespace fwc {

struct WildcardMatch
{
    virtual ~WildcardMatch() {};

    virtual bool isMatch(const std::string_view& text, const std::string& pattern) const = 0;
};

} // namespace fwc
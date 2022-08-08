#pragma once

#include <string>
#include <string_view>

struct WildcardMatch
{
    virtual ~WildcardMatch() {};

    virtual bool isMatch(const std::string_view& text, const std::string& pattern) const = 0;
};

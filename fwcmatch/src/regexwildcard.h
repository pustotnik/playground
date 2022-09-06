#pragma once

#include <regex>

#include "wildcard.h"

namespace fwc {

class REMatch final: public WildcardMatch
{
public:
    bool isMatch(const std::string_view& text, const std::string& pattern) const override;

private:
    // used as a simple thread safe cache
    thread_local static std::string _pattern;
    thread_local static std::regex  _regex;

    static void mkRegExPattern(const std::string& pattern);
};

} // namespace fwc
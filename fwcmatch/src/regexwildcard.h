#pragma once

#include <regex>

#include "wildcard.h"

namespace fwc {

class REMatch final: public WildcardMatch
{
private:
    bool isMatchImpl(const std::string_view& text,
                                const std::string& pattern) const override;

    static void mkRegExPattern(const std::string& pattern);

    // used as a simple thread safe cache
    thread_local static std::string _pattern;
    thread_local static std::regex  _regex;
};

} // namespace fwc
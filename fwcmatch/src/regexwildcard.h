#pragma once

#include <regex>

#include "wildcard.h"

class REMatch final: public WildcardMatch
{
public:
    bool isMatch(const std::string_view& text, const std::string& pattern) const override;

private:
    mutable std::string _pattern;
    mutable std::regex  _regex;
    mutable std::cmatch _matchResults;

    void mkRegExPattern(const std::string& pattern) const;
};

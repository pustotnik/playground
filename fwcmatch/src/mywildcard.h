#pragma once

#include "wildcard.h"

class MyWildcardMatch final: public WildcardMatch
{
public:
    bool isMatch(const std::string_view& text, const std::string& pattern) const override;
};
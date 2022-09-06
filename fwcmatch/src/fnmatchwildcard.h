#pragma once

#include "wildcard.h"

// Thread safe
class FNMatch final: public WildcardMatch
{
public:
    bool isMatch(const std::string_view& text, const std::string& pattern) const override;

private:
    // used as a cache to reduce number of memory allocation/deallocation
    thread_local static std::string _text;
};

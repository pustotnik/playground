#pragma once

#include "wildcard.h"

namespace fwc {

// Thread safe
class FNMatch final: public WildcardMatch
{
private:
    bool isMatchImpl(const std::string_view& text,
                                const std::string& pattern) const override;

    // used as a cache to reduce number of memory allocation/deallocation
    thread_local static std::string _text;
};

} // namespace fwc
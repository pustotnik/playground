#pragma once

#include "wildcard.h"

namespace fwc {

// Thread safe
class MyWildcardMatch final: public WildcardMatch
{
private:
    bool isMatchImpl(const std::string_view& text,
                                const std::string& pattern) const override;
};

} // namespace fwc
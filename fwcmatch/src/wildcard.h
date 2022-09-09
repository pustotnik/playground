#pragma once

#include <string>
#include <string_view>

namespace fwc {

class WildcardMatch
{
public:
    virtual ~WildcardMatch() {};

    bool isMatch(const std::string_view& text, const std::string& pattern) const {
        if(pattern.empty()) {
            // regard empty pattern as "*", linux grep conducts in the same way
            return true;
        }

        if(text.empty()) {
            // only * in pattern can match an empty text
            return (pattern.size() == 1 && pattern[0] == '*');
        }

        return isMatchImpl(text, pattern);
    }

private:
    virtual bool isMatchImpl(const std::string_view& text, const std::string& pattern) const = 0;
};

} // namespace fwc
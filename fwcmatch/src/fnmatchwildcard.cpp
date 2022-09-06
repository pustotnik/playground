
#include <fnmatch.h>

#include "fnmatchwildcard.h"

namespace fwc {

thread_local std::string FNMatch::_text;

bool FNMatch::isMatch(const std::string_view& text, const std::string& pattern) const {

    if(text.empty() || pattern.empty()) {
        return false;
    }

    // fnmatch expects C string terminated by a null character '\0' while we
    // have no such a string in the 'text' so I have to make a copy from the
    // string_view to a string with a null character '\0'.
    _text = text;

    return 0 == fnmatch(pattern.c_str(), _text.c_str(), FNM_NOESCAPE);
}

} // namespace fwc
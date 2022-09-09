
#include "mywildcard.h"
#include <cassert>

namespace fwc {

bool MyWildcardMatch::isMatchImpl(const std::string_view& text, const std::string& pattern) const {

    const char* ppattern = pattern.c_str();
    const char* ptext    = text.cbegin();

    const char* textpos = nullptr;
    const char* starpos = nullptr;

    while(ptext != text.cend()) {
        if(*ptext == *ppattern || '?' == *ppattern) {
            // Just go forward through the text and the pattern.
            ++ptext;
            ++ppattern;
        }
        else if('*' == *ppattern) {
            if(! *++ppattern) {
                // We have '*' at the end of the pattern and can just get out fast.
                // This allow us to speed up parsing of a long text with '*' at
                // the end in a pattern.
                return true;
            }

            starpos = ppattern;

            // We don't shift text pointer here because * also means zero characters.
            textpos = ptext;
        }
        else if(starpos) {
            // There was * so we return back to the old position with '*'.
            ppattern = starpos;
            // Shift saved text position and set to the current text pointer.
            ptext = ++textpos;
        }
        else {
            // match wasn't found
            return false;
        }
    }

    // Handle tail of '*' in the pattern: we went through the whole text but
    // not through the pattern and it can contain '*' at the end yet.
    while('*' == *ppattern) {
        ++ppattern;
    }

    // Return true if we went through the whole pattern.
    return !*ppattern;
}

} // namespace fwc
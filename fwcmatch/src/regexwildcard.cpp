
#include <regex>
#include <string_view>
#include <vector>
#include <utility>

#include "regexwildcard.h"

namespace fwc {

using string      = std::string;
using string_view = std::string_view;
using regex       = std::regex;

static const std::vector<std::pair<string, string>> RE_REPLACE = {

    { "\\", "\\\\" },
    { "^", "\\^" },
    { ".", "\\." },
    { "$", "\\$" },
    { "|", "\\|" },
    { "(", "\\(" },
    { ")", "\\)" },
    { "{", "\\{" },
    { "{", "\\}" },
    { "[", "\\[" },
    { "]", "\\]" },
    { "+", "\\+" },
    { "/", "\\/" },

    { "*", ".*" },
    { "?", "." },
};

thread_local string REMatch::_pattern;
thread_local regex REMatch::_regex;

static void replaceAll(string& str, const string& from, const string& to) {
    string::size_type pos = 0;
    while ( (pos = str.find(from, pos) ) != string::npos ) {
        str.replace(pos, from.size(), to);
        pos += to.size();
    }
}

bool REMatch::isMatchImpl(const string_view& text, const string& pattern) const {

    mkRegExPattern(pattern);
    return regex_match(text.cbegin(), text.cend(), _regex);
}

void REMatch::mkRegExPattern(const string& pattern) {
    if(pattern == _pattern) {
        return;
    }

    string repattern = _pattern = pattern;
    for(auto const& line: RE_REPLACE) {
        auto const& [from, to] = line;
        replaceAll(repattern, from, to);
    }

    //constexpr regex::flag_type flags = regex::optimize|regex::basic;
    //constexpr regex::flag_type flags = regex::optimize|regex::ECMAScript;
    constexpr regex::flag_type flags = regex::ECMAScript;
    _regex.assign(repattern, flags);
}

} // namespace fwc
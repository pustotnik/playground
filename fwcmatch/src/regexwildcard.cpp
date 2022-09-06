
#include <regex>
#include <vector>
#include <utility>

#include "regexwildcard.h"

using namespace std;

static const vector<pair<string, string>> RE_REPLACE = {

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

bool REMatch::isMatch(const string_view& text, const string& pattern) const {

    if(text.empty() || pattern.empty()) {
        return false;
    }

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
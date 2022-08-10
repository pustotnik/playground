#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "noncopyable.h"
#include "wildcard.h"
#include "filereader.h"

class SequentialProcessor final: private noncopyable
{
public:

    explicit SequentialProcessor(size_t maxLines);

    size_t execute(const std::string& filename, const std::string& pattern,
                WildcardMatch& wcmatch, FileReader& freader);

private:
    typedef std::string_view FileLine;

    size_t                        _maxLines;
    std::vector<char>             _buffer;
    mutable std::vector<FileLine> _flines;
};

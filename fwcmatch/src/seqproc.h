#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "noncopyable.h"
#include "linesblock.h"
#include "wildcard.h"
#include "filereader.h"

class SequentialProcessor final: private noncopyable
{
public:

    explicit SequentialProcessor(const size_t maxLines);

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

private:

    const size_t _maxLines;
    LinesBlock   _linesBlock;
};

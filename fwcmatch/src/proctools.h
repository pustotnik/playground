#pragma once

#include <cstddef>
#include <string>
#include <algorithm>

#include "linesblock.h"
#include "wildcard.h"
#include "filereader.h"

namespace fwc {
namespace proctools {

// Read file lines in a block
void readInLinesBlock(FileReader& freader, LinesBlock& block);

// Filter lines from a block
// Returns number of found lines according pattern
size_t filterBlock(WildcardMatch& wcmatch, const std::string& pattern,
                                                        LinesBlock const& block);

/// Inline implementation

inline size_t filterBlock(WildcardMatch& wcmatch,
                                    const std::string& pattern, LinesBlock const& block) {

    auto const& lines = block.lines();
    size_t counter = count_if(lines.cbegin(), lines.cend(),
        [&](auto const& line){ return wcmatch.isMatch(line, pattern); }
    );

    return counter;
}

} // namespace proctools
} // namespace fwc

#include <cassert>
#include <cstddef>
#include <algorithm>
#include <iostream>

#include "proctools.h"
#include "seqproc.h"

namespace fwc {

static constexpr size_t BLOCK_SIZE = 4*1024;

SequentialProcessor::SequentialProcessor(size_t maxLines, bool needsBuffer) {

    assert(maxLines > 0);
    _linesBlock.alloc(maxLines, needsBuffer, BLOCK_SIZE);
}

size_t SequentialProcessor::execute(FileReader& freader, const std::string& filename,
                            WildcardMatch& wcmatch, const std::string& pattern) {

    ScopedFileOpener fopener(freader, filename);

    size_t result = 0;
    for(;;) {
        proctools::readInLinesBlock(freader, _linesBlock);
        if(_linesBlock.lines().empty()) {
            // end of file
            break;
        }

        result += proctools::filterBlock(wcmatch, pattern, _linesBlock);
    }

    return result;
}

} // namespace fwc
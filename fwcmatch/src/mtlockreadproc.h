#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <mutex>

#include "noncopyable.h"
#include "linesblock.h"
#include "wildcard.h"
#include "filereader.h"

namespace fwc {

/*
This class implements may be simplest way to solve the problem with
multiple threads. It uses only one mutex and no queues. It's simple to
implement and understand. But such a way is not always possible and is not
always effective.
*/

class MTLockReadProcessor final: private noncopyable
{
public:

    MTLockReadProcessor(size_t numOfThreads, size_t maxLines, bool needsBuffer);

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

private:

    std::vector<LinesBlock> _linesBlocks;
    std::vector<size_t>     _counters;
    std::mutex              _mutex;
    const size_t            _numOfThreads;
};

} // namespace fwc
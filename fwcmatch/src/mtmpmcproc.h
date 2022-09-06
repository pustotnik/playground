#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "rigtorp/MPMCQueue.h"

#include "basepcproc.h"

namespace fwc {

/*
This class uses MPMCQueue from https://github.com/rigtorp/MPMCQueue.
*/

class MPMCProcessor final: public BaseProdConsProcessor
{
public:
    MPMCProcessor(size_t queueSize, size_t numOfConsThreads,
                                        size_t maxLines, bool needsBuffer);

private:

    using BlockPtrsQueue = rigtorp::MPMCQueue<LinesBlockPtr>;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch,
                                        const std::string& pattern) override;

    void init() override;

    LinesBlockPool             _blocksPool;
    BlockPtrsQueue             _blocksQueue;
    BlockPtrsQueue             _freeBlocks;
};

} // namespace fwc
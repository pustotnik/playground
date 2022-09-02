#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "dringbuffer.h"
#include "linesblock.h"
#include "basemtproc.h"

/*
This class implements alternative version of MTCondVarProcessor.
In theory this implemenation has better memory locality but uses more mutex locks
or copying of blocks if blocks without a buffer (mmap).
*/

class MTCondVarProcessor2 final: public BaseMTProcessor
{
public:
    MTCondVarProcessor2(size_t queueSize, size_t numOfConsThreads,
                        size_t maxLines, bool needsBuffer);

private:

    using BlocksRing = DRingBuffer<LinesBlock>;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch,
                                        const std::string& pattern) override;

    void init() override;

    BlocksRing              _blocksQueue;
    std::vector<LinesBlock> _localBlocks;
    std::mutex              _queueMutex;
    std::condition_variable _cvNonEmpty;
    std::condition_variable _cvNonFull;
    bool                    _stop        { false };
};

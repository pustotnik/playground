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
In theory this implemenation has better memory locality but uses more mutex locks.
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
    std::mutex              _queueMutex;
    std::condition_variable _cvNonEmpty;
    std::condition_variable _cvNonFull;
    const size_t            _maxLines;
    const bool              _needsBuffer;
    bool                    _stop        { false };
};

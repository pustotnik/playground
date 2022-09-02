#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "basemtproc.h"

/*
This class implements strategy of solving the problem with mutexes and
condition variables. There is no memory reallocation during processing and
it uses ring buffer for the queue of data between producer and consumers.
*/

class MTCondVarProcessor final: public BaseMTProcessor
{
public:
    MTCondVarProcessor(size_t queueSize, size_t numOfConsThreads,
                        size_t maxLines, bool needsBuffer);

private:

    using BlocksRing = SimpleRingBuffer<LinesBlock>;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch,
                                        const std::string& pattern) override;

    void init() override;

    BlocksRing              _blocksQueue;
    std::vector<LinesBlock> _firstBlocks;
    std::mutex              _queueMutex;
    std::condition_variable _cvNonEmpty;
    std::condition_variable _cvNonFull;
    bool                    _stop        { false };
};

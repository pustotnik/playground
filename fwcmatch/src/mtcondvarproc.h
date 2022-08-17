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
    MTCondVarProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines);

private:

    using BlockPtrsRing = SimpleRingBuffer<LinesBlockPtr>;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch,
                                        const std::string& pattern) override;

    size_t calcFinalResult() const override;
    void init() override;

    BlockPtrsRing           _blocksQueue;
    std::vector<size_t>     _counters;
    std::mutex              _queueMutex;
    std::condition_variable _cvNonEmpty;
    std::condition_variable _cvNonFull;
    bool                    _stop { false };
};

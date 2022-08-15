#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "baseprodconsproc.h"

/*
This class implements strategy of solving the problem with mutexes and
condition variables. There is no memory reallocation during processing and
it uses ring buffer for the queue of data between producer and consumers.
*/

class ProdConsProcessor final: public BaseProdConsProcessor
{
public:
    ProdConsProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines);

private:

    typedef SimpleRingBuffer<LinesBlockPtr> BlockPtrsRing;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch, std::string pattern) override;

    size_t calcFinalResult() const override;
    void init() override;

    BlockPtrsRing           _blocksQueue;
    std::vector<size_t>     _counters;
    std::mutex              _queueMutex;
    std::condition_variable _cvNonEmpty;
    std::condition_variable _cvNonFull;
    bool                    _stop { false };
};

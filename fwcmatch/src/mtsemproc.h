#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <semaphore>

#include "basemtproc.h"

/*
This class implements strategy of solving the problem with mutexes and
semaphores. There is no memory reallocation during processing and
it uses ring buffer for the queue of data between producer and consumers.
*/

class MTSemProcessor final: public BaseMTProcessor
{
public:
    MTSemProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines);

private:

    typedef SimpleRingBuffer<LinesBlockPtr> BlockPtrsRing;
    typedef std::counting_semaphore<>       Semaphore;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch, std::string pattern) override;

    size_t calcFinalResult() const override;
    void init() override;

    BlockPtrsRing              _blocksQueue;
    std::vector<size_t>        _counters;
    std::mutex                 _queueMutex;
    std::unique_ptr<Semaphore> _semEmpty;
    std::unique_ptr<Semaphore> _semFull;
};

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
    MTSemProcessor(size_t queueSize, size_t numOfConsThreads,
                                        size_t maxLines, bool needsBuffer);

private:

    using BlockPtrsRing = SimpleRingBuffer<LinesBlockPtr>;
    using Semaphore     = std::counting_semaphore<>;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch,
                                        const std::string& pattern) override;

    void init() override;

    LinesBlockPool             _blocksPool;
    BlockPtrsRing              _blocksQueue;
    std::vector<LinesBlockPtr> _firstBlocks;
    std::mutex                 _queueMutex;
    std::unique_ptr<Semaphore> _semEmpty;
    std::unique_ptr<Semaphore> _semFull;
    const size_t               _numOfThreads;
    const bool                 _needsBuffer;
};

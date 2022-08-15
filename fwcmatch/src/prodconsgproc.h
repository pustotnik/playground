#pragma once

#include <cstddef>
#include <memory>
#include <atomic>

#include "wfringbuffer.h"
#include "baseprodconsproc.h"

/*
This class implements strategy of solving the problem as a group of
pairs <single producer - single consumer> with wait-free ring buffer.
THis solution don't use mutexes and condition variables for communication
between producer and consumers but it works in busy-waiting mode when
these ring buffers are full or empty.

There is no memory reallocation during processing.
*/
class ProdConsGProcessor final: public BaseProdConsProcessor
{
public:
    ProdConsGProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines);

private:

    typedef WFSimpleRingBuffer<LinesBlockPtr> WFBlockPtrsRing; // wait free ring buffer

    // specific info for each consumer's thread
    struct ConsumerInfo final {
        WFBlockPtrsRing blocksQueue;
        size_t          counter {0};

        explicit ConsumerInfo(size_t queueSize): blocksQueue(queueSize) {}

        // not thread safe
        void clear() {
            blocksQueue.clear();
            counter = 0;
        }
    };

    typedef std::unique_ptr<ConsumerInfo> ConsumerInfoUPtr;
    typedef std::vector<ConsumerInfoUPtr> VectorOfConsumerInfo;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch, std::string pattern) override;

    size_t calcFinalResult() const override;
    void init() override;

    VectorOfConsumerInfo _consThreadInfo;
    std::atomic<bool>    _stop { false };
};

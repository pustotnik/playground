#pragma once

#include <cstddef>
#include <memory>
#include <atomic>

#include "wfringbuffer.h"
#include "basemtproc.h"

/*
This class implements strategy of solving the problem as a group of
pairs <single producer - single consumer> with wait-free ring buffer.
This solution don't use mutexes and condition variables for communication
between producer and consumers but it works in busy-waiting mode when
these ring buffers are full or empty.

This is not the best and fastest solution of such type but it can show the main
disadvantage of this method.

There is no memory reallocation during processing.
*/
class MTLockFreeProcessor final: public BaseMTProcessor
{
public:
    MTLockFreeProcessor(size_t queueSize, size_t numOfConsThreads,
                                    size_t maxLines, bool needsBuffer);

private:

    using WFBlockRing = WFSimpleRingBuffer<LinesBlock>; // wait free ring buffer

    // specific info for each consumer's thread
    struct ConsumerInfo final {
        WFBlockRing     blocksQueue;
        size_t          counter {0};

        explicit ConsumerInfo(size_t queueSize): blocksQueue(queueSize) {}

        // not thread safe
        void reset() {
            blocksQueue.reset();
            counter = 0;
        }
    };

    using ConsumerInfoUPtr     = std::unique_ptr<ConsumerInfo>;
    using VectorOfConsumerInfo = std::vector<ConsumerInfoUPtr>;

    void readFileLines(FileReader& freader) override;
    void filterLines(size_t idx, WildcardMatch& wcmatch,
                                    const std::string& pattern) override;

    size_t calcFinalResult() const override;
    void init() override;

    VectorOfConsumerInfo _consThreadInfo;
    std::atomic<bool>    _stop { false };
};

inline size_t MTLockFreeProcessor::calcFinalResult() const {
    size_t result = 0;
    for(auto const& consInfo: _consThreadInfo) {
        result += consInfo->counter;
    }
    return result;
}

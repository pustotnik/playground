
#include <cassert>
#include <thread>

#include "proctools.h"
#include "mtlockfreeproc.h"

namespace fwc {

MTLockFreeProcessor::MTLockFreeProcessor(size_t queueSize, size_t numOfConsThreads,
                                        size_t maxLines, bool needsBuffer):
    BaseProdConsProcessor(numOfConsThreads) {

    assert(queueSize > 0);

    _consThreadInfo.reserve(numOfConsThreads);
    for(size_t i = 0; i < numOfConsThreads; ++i) {
        auto cinfo = std::make_unique<ConsumerInfo>(queueSize);
        cinfo->blocksQueue.apply([&](LinesBlock& block) {
            block.alloc(maxLines, needsBuffer);
        });
        _consThreadInfo.push_back(std::move(cinfo));
    }
}

void MTLockFreeProcessor::init() {

    _stop.store(false, std::memory_order_release);

    for(auto& consInfo: _consThreadInfo) {
        consInfo->reset();
    }
}

void MTLockFreeProcessor::readFileLines(FileReader& freader) {

    const auto numOfConsThreads = _consThreadInfo.size();
    const size_t maxFailedPushes = numOfConsThreads * 1000;

    size_t failedPushes = 0;
    size_t consumerIdx = 0;

    bool noData = false;
    auto readBlock = [&](LinesBlock& block) {
        proctools::readInLinesBlock(freader, block);
        noData = block.lines().empty();
    };

    for(;;) {

        auto& consInfo = *_consThreadInfo[consumerIdx];

        if(consInfo.blocksQueue.push(readBlock)) {
            if(noData) {
                // end of file
                break;
            }
            failedPushes = 0;
            continue;
        }

        consumerIdx = ++consumerIdx < numOfConsThreads ? consumerIdx: 0;

        if(++failedPushes < maxFailedPushes) {
            continue;
        }

        // usually it is useless function on a platform with more than one
        // CPU core but because of busy-waiting it helps to decrease CPU load
        std::this_thread::yield();
    }

    _stop.store(true, std::memory_order_release);
}

void MTLockFreeProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const std::string& pattern) {

    constexpr size_t maxSpins = 1000;
    auto& consInfo = *_consThreadInfo[idx];
    size_t counter = 0;
    size_t spinner = 0;

    auto handleBlock = [&](LinesBlock const& block) {
        counter += proctools::filterBlock(wcmatch, pattern, block);
    };

    for(;;) {

        if(consInfo.blocksQueue.pop(handleBlock)) {
            continue;
        }

        if(_stop.load(std::memory_order_acquire)) {
            break;
        }

        if(++spinner > maxSpins) {

            // usually it is useless function on a platform with more than one
            // CPU core but because of busy-waiting it helps to decrease CPU load
            std::this_thread::yield();
            spinner = 0;
        }

        // spin
    }

    consInfo.counter = counter;
}

} // namespace fwc
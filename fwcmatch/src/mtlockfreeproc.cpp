
#include <cassert>
#include <thread>

#include "mtlockfreeproc.h"

using namespace std;

MTLockFreeProcessor::MTLockFreeProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
BaseMTProcessor(numOfConsThreads, maxLines,
    // for each block in queue and for each thread for waiting
    (queueSize + 1) * (numOfConsThreads + 1)) {

    assert(queueSize > 0);

    _consThreadInfo.reserve(numOfConsThreads);
    for(size_t i = 0; i < numOfConsThreads; ++i) {
        _consThreadInfo.push_back(make_unique<ConsumerInfo>(queueSize));
    }
}

void MTLockFreeProcessor::init() {

    _stop.store(false, memory_order_release);

    for(auto& consInfo: _consThreadInfo) {
        consInfo->clear();
    }
}

size_t MTLockFreeProcessor::calcFinalResult() const {
    size_t result = 0;
    for(auto const& consInfo: _consThreadInfo) {
        result += consInfo->counter;
    }
    return result;
}

void MTLockFreeProcessor::readFileLines(FileReader& freader) {

    const auto numOfConsThreads = _consThreadInfo.size();
    const size_t maxFailedPushes = numOfConsThreads * 2;

    size_t failedPushes = 0;
    size_t consumerIdx = 0;
    LinesBlockPtr block = nullptr;

    for(;;) {

        if(!block) {
            block = readInLinesBlock(freader);
            if(block->lines.empty()) {
                // end of file
                break;
            }
        }

        auto& consInfo = *_consThreadInfo[consumerIdx];
        if(consInfo.blocksQueue.push(block)) {
            block = nullptr;
            failedPushes = 0;
            continue;
        }

        consumerIdx = ++consumerIdx < numOfConsThreads ? consumerIdx: 0;

        if(++failedPushes < maxFailedPushes) {
            continue;
        }

        // usually it is useless function on a platform with more than one
        // CPU core but because of busy-waiting it helps to decrease CPU load
        this_thread::yield();
    }

    _stop.store(true, memory_order_release);
}

void MTLockFreeProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, string pattern) {

    size_t counter = 0;
    LinesBlockPtr block = nullptr;

    auto& consInfo = *_consThreadInfo[idx];

    for(;;) {

        if(!consInfo.blocksQueue.pop(block)) {

            if(_stop.load(memory_order_acquire)) {
                break;
            }

            // usually it is useless function on a platform with more than one
            // CPU core but because of busy-waiting it helps to decrease CPU load
            this_thread::yield();

            // spin
            continue;
        }

        assert(block);
        counter += filterBlockAndFree(wcmatch, pattern, *block);
    }

    consInfo.counter = counter;
}
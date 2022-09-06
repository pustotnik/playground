
#include <cassert>

#include "utils.h"
#include "proctools.h"
#include "mtcondvarproc2.h"

namespace fwc {

MTCondVarProcessor2::MTCondVarProcessor2(size_t queueSize, size_t numOfConsThreads,
                                        size_t maxLines, bool needsBuffer):
    BaseProdConsProcessor(numOfConsThreads),
    _blocksQueue(queueSize, numOfConsThreads),
    _needsBuffer(needsBuffer) {

    assert(queueSize > 0);

    _blocksQueue.apply([&](LinesBlock& block) {
        block.alloc(maxLines, needsBuffer);
    });

    auto numOfThreads = numOfConsThreads + 1;
    _localBlocks.reserve(numOfThreads);
    for(size_t i = 0; i < numOfThreads; ++i) {
        // these blocks are used only when needsBuffer == false
        _localBlocks.emplace_back(maxLines, false);
    }
}

void MTCondVarProcessor2::init() {

    _stop = false;
    _blocksQueue.reset();
}

void MTCondVarProcessor2::readFileLines(FileReader& freader) {

    auto waitIfFull = [&](auto& lock) {
        if(_blocksQueue.full()) {
            _cvNonFull.wait(lock, [&](){ return !_blocksQueue.full(); });
        }
    };

    if(_needsBuffer) {
        LinesBlockPtr block = nullptr;
        for(;;) {

            std::unique_lock<std::mutex> lock(_queueMutex);
            waitIfFull(lock);

            block = _blocksQueue.enqueuePrepare();
            lock.unlock();

            proctools::readInLinesBlock(freader, *block);
            if(block->lines().empty()) {
                break;
            }

            lock.lock();
            _blocksQueue.enqueueCommit();
            _cvNonEmpty.notify_one();
        }
    }
    else {
        auto& block = _localBlocks[0];
        for(;;) {
            proctools::readInLinesBlock(freader, block);
            if(block.lines().empty()) {
                break;
            }

            std::unique_lock<std::mutex> lock(_queueMutex);
            waitIfFull(lock);

            *_blocksQueue.enqueuePrepare() = block;
            _blocksQueue.enqueueCommit();
            _cvNonEmpty.notify_one();
        }
    }

    std::scoped_lock lock(_queueMutex);
    _stop = true;
    _cvNonEmpty.notify_all();
}

void MTCondVarProcessor2::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const std::string& pattern) {

    size_t counter = 0;
    LinesBlockPtr block = nullptr;
    auto& blockCopy = _localBlocks[idx + 1];

    for(;;) {

        std::unique_lock<std::mutex> lock(_queueMutex);
        if(_blocksQueue.empty()) {
            _cvNonEmpty.wait(lock, [&](){ return !_blocksQueue.empty() || _stop; });
            if(_stop) {
                break;
            }
        }

        block = _blocksQueue.dequeuePrepare(idx);
        if(_needsBuffer) {
            lock.unlock();

            counter += proctools::filterBlock(wcmatch, pattern, *block);

            lock.lock();
            _blocksQueue.dequeueCommit(idx);
            if(_blocksQueue.size() <= 1) {
                _cvNonFull.notify_one();
            }
        }
        else {
            // in this case it is faster to copy block
            blockCopy = *block;
            _blocksQueue.dequeueCommit(idx);
            if(_blocksQueue.empty()) {
                _cvNonFull.notify_one();
            }
            lock.unlock();

            counter += proctools::filterBlock(wcmatch, pattern, blockCopy);
        }
    }

    _counters[idx] = counter;
}

} // namespace fwc

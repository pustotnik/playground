
#include <cassert>

#include "mtcondvarproc2.h"
#include "utils.h"

using namespace std;

MTCondVarProcessor2::MTCondVarProcessor2(size_t queueSize, size_t numOfConsThreads,
                                        size_t maxLines, bool needsBuffer):
    BaseMTProcessor(numOfConsThreads, maxLines),
    _blocksQueue(queueSize, numOfConsThreads),
    _maxLines(maxLines),
    _needsBuffer(needsBuffer) {

    assert(queueSize > 0);

    _blocksQueue.apply([&](LinesBlock& block) {
        block.lines.reserve(maxLines);
        if(needsBuffer) {
            block.buffer.resize(maxLines);
        }
    });
}

void MTCondVarProcessor2::init() {

    _stop = false;
    _blocksQueue.reset();
}

void MTCondVarProcessor2::readFileLines(FileReader& freader) {

    LinesBlockPtr block = nullptr;

    for(;;) {

        unique_lock<mutex> lock(_queueMutex);
        if(_blocksQueue.full()) {
            _cvNonFull.wait(lock, [&](){ return !_blocksQueue.full(); });
        }

        block = _blocksQueue.enqueuePrepare();
        lock.unlock();

        readInLinesBlock(freader, *block);
        if(block->lines.empty()) {
            break;
        }

        lock.lock();
        _blocksQueue.enqueueCommit();
        _cvNonEmpty.notify_one();
    }

    scoped_lock lock(_queueMutex);
    _stop = true;
    _cvNonEmpty.notify_all();
}

void MTCondVarProcessor2::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const string& pattern) {

    size_t counter = 0;
    LinesBlockPtr block = nullptr;
    bool notify;

    for(;;) {

        unique_lock<mutex> lock(_queueMutex);
        if(_blocksQueue.empty()) {
            _cvNonEmpty.wait(lock, [&](){ return !_blocksQueue.empty() || _stop; });
            if(_stop) {
                break;
            }
        }

        block = _blocksQueue.dequeuePrepare(idx);
        lock.unlock();

        counter += filterBlock(wcmatch, pattern, *block);

        lock.lock();
        _blocksQueue.dequeueCommit(idx);
        notify = _needsBuffer ? _blocksQueue.size() <= 1 : _blocksQueue.empty();
        if(notify) {
            _cvNonFull.notify_one();
        }
    }

    _counters[idx] = counter;
}

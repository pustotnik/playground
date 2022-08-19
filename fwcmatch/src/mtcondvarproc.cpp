
#include <cassert>
#include <algorithm>

#include "mtcondvarproc.h"
#include "utils.h"

using namespace std;

MTCondVarProcessor::MTCondVarProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
    BaseMTProcessor(numOfConsThreads, maxLines),
    // for each block in queue and for each thread for waiting
    _blocksPool(queueSize + numOfConsThreads + 1, maxLines),
    _blocksQueue(queueSize),
    _counters(numOfConsThreads, 0)  {

    assert(queueSize > 0);
}

void MTCondVarProcessor::init(const bool needsBuffer) {

    _stop = false;
    _blocksQueue.clear();
    _blocksPool.reset(needsBuffer);

    // std::fill works too slowly :(
    _counters.assign(_counters.size(), 0);
}

void MTCondVarProcessor::readFileLines(FileReader& freader) {

    LinesBlockPtr block = nullptr;

    for(;;) {

        block = allocBlock(_blocksPool, _blocksMutex);
        assert(block);

        readInLinesBlock(freader, *block);
        if(block->lines.empty()) {
            // end of file
            break;
        }

        unique_lock<mutex> lock(_queueMutex);
        if(_blocksQueue.full()) {
            _cvNonFull.wait(lock, [&](){ return !_blocksQueue.full(); });
        }
        _blocksQueue.push(block);
        _cvNonEmpty.notify_one();
    }

    scoped_lock lock(_queueMutex);
    _stop = true;
    _cvNonEmpty.notify_all();
}

void MTCondVarProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const string& pattern) {

    size_t counter = 0;
    LinesBlockPtr block = nullptr;

    for(;;) {

        unique_lock<mutex> lock(_queueMutex);
        if(_blocksQueue.empty()) {
            _cvNonEmpty.wait(lock, [&](){ return !_blocksQueue.empty() || _stop; });
            if(_stop) {
                break;
            }
        }

        block = _blocksQueue.top();
        _blocksQueue.pop();
        if(_blocksQueue.empty()) {
            _cvNonFull.notify_one();
        }
        lock.unlock();

        assert(block);
        counter += filterBlock(wcmatch, pattern, *block);
        freeBlock(_blocksPool, _blocksMutex, block);
    }

    _counters[idx] = counter;
}

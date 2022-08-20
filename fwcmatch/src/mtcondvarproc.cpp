
#include <cassert>
#include <algorithm>

#include "mtcondvarproc.h"
#include "utils.h"

using namespace std;

MTCondVarProcessor::MTCondVarProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
    BaseMTProcessor(numOfConsThreads, maxLines),
    _blocksQueue(queueSize),
    _counters(numOfConsThreads, 0),
    _maxLines(maxLines)  {

    assert(queueSize > 0);
}

void MTCondVarProcessor::init(const bool needsBuffer) {

    _stop = false;
    _needsBuffer = needsBuffer;

    _blocksQueue.clear();
    _blocksQueue.apply([&](LinesBlock& block) { initLinesBlock(block); } );

    // std::fill works too slowly :(
    _counters.assign(_counters.size(), 0);
}

/*
We don't need to make copy of block before pushing/after poping in the
ring buffer. It is more effective to make swap between a local block and
a block in the ring buffer under existing mutex lock. In this case only one
thread writes block at the same time and only one thread reads block
at the same time because it is always thread local block.
C++ std:mutex lock and unlock together imply memory fence and
guarantee visibility of these changes in all threads:
    >> A synchronization operation without an associated memory location
    >> is a fence and can be either an acquire fence, a release fence,
    >> or both an acquire and release fence.

    >> For example, a call that acquires a mutex will perform an acquire
    >> operation on the locations comprising the mutex. Correspondingly,
    >> a call that releases the same mutex will perform a release operation
    >> on those same locations.

    See also section "Release-Acquire ordering" here:
    https://en.cppreference.com/w/cpp/atomic/memory_order
*/

void MTCondVarProcessor::readFileLines(FileReader& freader) {

    LinesBlock block;
    initLinesBlock(block);

    for(;;) {

        readInLinesBlock(freader, block);
        if(block.lines.empty()) {
            // end of file
            break;
        }

        unique_lock<mutex> lock(_queueMutex);
        if(_blocksQueue.full()) {
            _cvNonFull.wait(lock, [&](){ return !_blocksQueue.full(); });
        }
        // we don't need to make copy of block here
        _blocksQueue.pushSwapping(block);
        _cvNonEmpty.notify_one();
    }

    scoped_lock lock(_queueMutex);
    _stop = true;
    _cvNonEmpty.notify_all();
}

void MTCondVarProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const string& pattern) {

    size_t counter = 0;
    LinesBlock block;
    initLinesBlock(block);

    for(;;) {

        unique_lock<mutex> lock(_queueMutex);
        if(_blocksQueue.empty()) {
            _cvNonEmpty.wait(lock, [&](){ return !_blocksQueue.empty() || _stop; });
            if(_stop) {
                break;
            }
        }

        // we don't need to make copy of block here
        block.swap(_blocksQueue.top());
        // return swapped block in the ring buffer as a free block to write
        _blocksQueue.pop();

        if(_blocksQueue.empty()) {
            _cvNonFull.notify_one();
        }
        lock.unlock();

        counter += filterBlock(wcmatch, pattern, block);
    }

    _counters[idx] = counter;
}


#include <cassert>
#include <algorithm>
#include <type_traits>
#include <iostream>

#include "mtcondvar2proc.h"
#include "utils.h"

using namespace std;

MTCondVar2Processor::MTCondVar2Processor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
    BaseMTProcessor(numOfConsThreads, maxLines),
    _blocksQueue(queueSize),
    _counters(numOfConsThreads, 0),
    _maxLines(maxLines)  {

    assert(queueSize > 0);
}

void MTCondVar2Processor::init(const bool needsBuffer) {

    _stop = false;
    _needsBuffer = needsBuffer;

    _blocksQueue.clear();
    _blocksQueue.apply([&](LinesBlock& block) { initLinesBlock(block); } );

    // std::fill works too slowly :(
    _counters.assign(_counters.size(), 0);
}

void MTCondVar2Processor::readFileLines(FileReader& freader) {

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
        _blocksQueue.pushSwapping(block);
        _cvNonEmpty.notify_one();
    }

    scoped_lock lock(_queueMutex);
    _stop = true;
    _cvNonEmpty.notify_all();
}

void MTCondVar2Processor::filterLines(size_t idx,
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

        block.swap(_blocksQueue.top());
        _blocksQueue.pop();
        if(_blocksQueue.empty()) {
            _cvNonFull.notify_one();
        }
        lock.unlock();

        counter += filterBlock(wcmatch, pattern, block);
    }

    _counters[idx] = counter;
}

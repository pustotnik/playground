
#include <cassert>
#include <algorithm>

#include "mtcondvarproc.h"

using namespace std;

MTCondVarProcessor::MTCondVarProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
BaseMTProcessor(numOfConsThreads, maxLines,
    // for each block in queue and for each thread for waiting
    queueSize + numOfConsThreads + 1),
_blocksQueue(queueSize), _counters(numOfConsThreads, 0)  {

    assert(queueSize > 0);
}

void MTCondVarProcessor::init() {

    _stop = false;
    _blocksQueue.clear();

    // std::fill works too slowly :(
    _counters.assign(_counters.size(), 0);
}

size_t MTCondVarProcessor::calcFinalResult() const {
    size_t result = 0;
    for(auto c: _counters) {
        result += c;
    }
    return result;
}

void MTCondVarProcessor::readFileLines(FileReader& freader) {

    for(;;) {

        auto block = readInLinesBlock(freader);
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
                            WildcardMatch& wcmatch, string pattern) {

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
        counter += filterBlockAndFree(wcmatch, pattern, *block);
    }

    _counters[idx] = counter;
}

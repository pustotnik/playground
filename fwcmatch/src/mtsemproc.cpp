
#include <cassert>
#include <algorithm>
#include <memory>
#include <numeric>
#include <iostream>

#include "mtsemproc.h"

using namespace std;

// special pointer to send as a terminal block
static void* TERM_BLOCK = reinterpret_cast<void*>(-1);

MTSemProcessor::MTSemProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
BaseMTProcessor(numOfConsThreads, maxLines,
    // for each block in queue and for each thread for waiting
    queueSize + numOfConsThreads + 1),
_blocksQueue(queueSize), _counters(numOfConsThreads, 0)  {

    assert(queueSize > 0);
}

void MTSemProcessor::init() {

    // I didn't find any other way to reset std::counting_semaphore objects
    _semEmpty = make_unique<Semaphore>(_blocksQueue.capacity());
    _semFull  = make_unique<Semaphore>(0);

    _blocksQueue.clear();

    // std::fill works too slowly :(
    _counters.assign(_counters.size(), 0);
}

size_t MTSemProcessor::calcFinalResult() const {
    return std::accumulate(_counters.begin(), _counters.end(),
                                decltype(_counters)::value_type(0));
}

void MTSemProcessor::readFileLines(FileReader& freader) {

    static auto termBlock = static_cast<LinesBlockPtr>(TERM_BLOCK);
    bool last = false;

    for(;;) {

        auto block = readInLinesBlock(freader);
        if(block->lines.empty()) {
            // end of file

            // use terminal block in the queue as a signal to stop consumers
            block = termBlock;
            last = true;
        }

        _semEmpty->acquire();
        {
            scoped_lock lock(_queueMutex);
            _blocksQueue.push(block);
        }
        _semFull->release();

        if(last) {
            // terminal block is always last in the queue
            break;
        }
    }
}

void MTSemProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, string pattern) {

    static auto termBlock = static_cast<LinesBlockPtr>(TERM_BLOCK);
    size_t counter = 0;
    LinesBlockPtr block = nullptr;
    bool last = false;

    for(;;) {

        // To stop a consumer the terminal block is used.

        _semFull->acquire();

        {
            scoped_lock lock(_queueMutex);
            block = _blocksQueue.top();
            last = (termBlock == block);
            if(!last) {
                _blocksQueue.pop();
            }
            // else keep the terminal block in the queue otherwise
            // other consumers won't stop
        }

        _semEmpty->release();

        if(last) {
            // unlock to stop other consumer's threads
            _semFull->release();
            break;
        }

        assert(block);
        counter += filterBlockAndFree(wcmatch, pattern, *block);
    }

    _counters[idx] = counter;
}

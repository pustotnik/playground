
#include <cassert>

#include "mtsemproc.h"

using namespace std;

// special pointer to send as a terminal block
static LinesBlockPtr TERM_BLOCK = reinterpret_cast<LinesBlockPtr>(-1);

MTSemProcessor::MTSemProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
    BaseMTProcessor(numOfConsThreads, maxLines),
    // for each block in queue and for each thread for waiting
    _blocksPool(queueSize + numOfConsThreads + 1, maxLines),
    _blocksQueue(queueSize),
    _counters(numOfConsThreads, 0),
    _numOfThreads(numOfConsThreads + 1)  {

    assert(queueSize > 0);
}

void MTSemProcessor::init(const bool needsBuffer) {

    _blocksPool.reset(needsBuffer);

    // I didn't find any other way to reset std::counting_semaphore objects
    _semEmpty = make_unique<Semaphore>(_blocksQueue.capacity());
    _semFull  = make_unique<Semaphore>(0);

    _firstBlocks.clear();
    for(size_t i = 0; i < _numOfThreads; ++i) {
        _firstBlocks.push_back(_blocksPool.allocBlock());
    }

    // fill ring buffer with valid pointers
    _blocksQueue.reset();
    for(size_t i = 0; i < _blocksQueue.capacity(); ++i) {
        _blocksQueue.push(_blocksPool.allocBlock());
    }
    // ring buffer must be empty
    _blocksQueue.reset();

    // std::fill works too slowly :(
    _counters.assign(_counters.size(), 0);
}

void MTSemProcessor::readFileLines(FileReader& freader) {

    bool last = false;
    auto block = _firstBlocks[0];

    for(;;) {

        assert(block);
        readInLinesBlock(freader, *block);
        if(block->lines.empty()) {
            // end of file

            // use terminal block in the queue as a signal to stop consumers
            block = TERM_BLOCK;
            last = true;
        }

        _semEmpty->acquire();
        {
            scoped_lock lock(_queueMutex);
            _blocksQueue.pushSwapping(block);
        }
        _semFull->release();

        if(last) {
            // terminal block is always last in the queue
            break;
        }
    }
}

void MTSemProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const string& pattern) {

    size_t counter = 0;
    LinesBlockPtr block = _firstBlocks[idx + 1];
    bool last = false;

    for(;;) {

        // To stop a consumer the terminal block is used.

        _semFull->acquire();

        {
            scoped_lock lock(_queueMutex);
            last = (TERM_BLOCK == _blocksQueue.top());
            if(!last) {
                std::swap(block, _blocksQueue.top());
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
        counter += filterBlock(wcmatch, pattern, *block);
    }

    _counters[idx] = counter;
}

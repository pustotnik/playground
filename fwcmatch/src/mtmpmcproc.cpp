
#include <cassert>

#include "mtmpmcproc.h"

using namespace std;

// special pointer to send as a terminal block
static LinesBlockPtr TERM_BLOCK = reinterpret_cast<LinesBlockPtr>(-1);

MPMCProcessor::MPMCProcessor(size_t queueSize, size_t numOfConsThreads,
                                            size_t maxLines, bool needsBuffer):
    BaseMTProcessor(numOfConsThreads, maxLines, needsBuffer),
    // for each block in queue and for each thread for waiting
    _blocksPool(queueSize + numOfConsThreads + 1, maxLines),
    _blocksQueue(queueSize),
    _freeBlocks(_blocksPool.capacity()) {

    assert(queueSize > 0);

    _blocksPool.reset(needsBuffer);
}

void MPMCProcessor::init() {

    _blocksPool.reset(false); // there is no need to allocate buffer here

    LinesBlockPtr tmp;
    while(!_blocksQueue.empty()) {
        _blocksQueue.pop(tmp);
    }

    while(!_freeBlocks.empty()) {
        _freeBlocks.pop(tmp);
    }
    for(size_t i = 0; i < _blocksPool.capacity(); ++i) {
        _freeBlocks.push(_blocksPool.allocBlock());
    }
}

void MPMCProcessor::readFileLines(FileReader& freader) {

    bool last = false;
    LinesBlockPtr block = nullptr;

    for(;;) {

        _freeBlocks.pop(block);

        assert(block);
        readInLinesBlock(freader, *block);
        if(block->lines.empty()) {
            // end of file

            // use terminal block in the queue as a signal to stop consumers
            _freeBlocks.push(block);
            block = TERM_BLOCK;
            last = true;
        }

        _blocksQueue.push(block);

        if(last) {
            // terminal block is always last in the queue
            break;
        }
    }
}

void MPMCProcessor::filterLines(size_t idx,
                            WildcardMatch& wcmatch, const string& pattern) {

    size_t counter = 0;
    LinesBlockPtr block = nullptr;

    for(;;) {

        // To stop a consumer the terminal block is used.

        _blocksQueue.pop(block);
        if(TERM_BLOCK == block) {
            // Unlock to stop other consumer's threads
            // Keep the terminal block in the queue otherwise
            // other consumers won't stop
            _blocksQueue.push(block);
            break;
        }

        assert(block);
        counter += filterBlock(wcmatch, pattern, *block);
        _freeBlocks.push(block);
    }

    _counters[idx] = counter;
}

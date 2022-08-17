#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "noncopyable.h"
#include "common.h"
#include "ringbuffer.h"
#include "wildcard.h"
#include "filereader.h"

class BaseMTProcessor: private noncopyable
{
public:

    BaseMTProcessor(size_t numOfConsThreads, size_t maxLines, size_t numOfBlocks);
    virtual ~BaseMTProcessor();

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

protected:

    // simplified structure for effective storage of blocks of file lines
    struct LinesBlock {
        std::vector<char> buffer;
        FileLineRefs      lines;
    };

    typedef LinesBlock* LinesBlockPtr;

    // must be used in readFileLines
    LinesBlockPtr readInLinesBlock(FileReader& freader);

    // must be used in filterLines
    size_t filterBlockAndFree(WildcardMatch& wcmatch, std::string pattern, LinesBlock& block);

private:
    typedef std::vector<LinesBlock>           VectorOfBlocks;
    typedef SimpleRingBuffer<LinesBlockPtr>   BlockPtrsRing;
    typedef std::vector<std::thread>          Threads;

    // it is called in the 'execute' method in the beginning (so threads haven't started yet)
    virtual void init() = 0;

    // it is called in producer thread
    virtual void readFileLines(FileReader& freader) = 0;

    // it is called in consumer threads
    virtual void filterLines(size_t idx, WildcardMatch& wcmatch, std::string pattern) = 0;

    // gather all counters from all consumers
    // it is called in the 'execute' method after all threads finished
    virtual size_t calcFinalResult() const = 0;

    void doInit();
    LinesBlockPtr allocBlock();
    void freeBlock(LinesBlockPtr p);

    const size_t            _numOfThreads;
    const size_t            _maxLines;
    VectorOfBlocks          _blocksHolder;
    BlockPtrsRing           _freeBlocks;
    Threads                 _threads;
    mutable std::mutex      _blockMutex;
};

inline size_t BaseMTProcessor::filterBlockAndFree(WildcardMatch& wcmatch,
                                        std::string pattern, LinesBlock& block) {

    auto& lines = block.lines;
    size_t counter = count_if(lines.cbegin(), lines.cend(),
        [&](auto const& line){ return wcmatch.isMatch(line, pattern); }
    );

    lines.clear();
    freeBlock(&block);

    return counter;
}

inline BaseMTProcessor::LinesBlockPtr BaseMTProcessor::allocBlock() {
    std::scoped_lock lock(_blockMutex);
    if(_freeBlocks.empty()) {
        return nullptr;
    }

    auto p = _freeBlocks.top();
    _freeBlocks.pop();
    return p;
}

inline void BaseMTProcessor::freeBlock(BaseMTProcessor::LinesBlockPtr p) {
    std::scoped_lock lock(_blockMutex);
    _freeBlocks.push(p);
}

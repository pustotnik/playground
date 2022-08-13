#pragma once

#include <cstddef>
#include <queue>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "noncopyable.h"
#include "common.h"
#include "ringbuffer.h"
#include "wildcard.h"
#include "filereader.h"

class ProdConsProcessor final: private noncopyable
{
public:

    ProdConsProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines);

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

private:

    // simplified structure for effective storage of blocks of file lines
    struct LinesBlock {
        std::vector<char> buffer;
        FileLineRefs      lines;
    };

    typedef LinesBlock*                     LinesBlockPtr;
    typedef std::vector<LinesBlock>         VectorOfBlocks;
    typedef std::vector<LinesBlockPtr>      VectorOfBlockPtrs;
    typedef SimpleRingBuffer<LinesBlockPtr> BlockPtrsRing;
    typedef std::vector<std::thread>        Threads;

    void readFileLines(FileReader& freader);
    void filterLines(size_t idx, WildcardMatch& wcmatch, std::string pattern);

    LinesBlockPtr allocBlock();
    void freeBlock(LinesBlockPtr p);

    void clear();

    size_t                  _numOfThreads;
    size_t                  _maxLines;
    VectorOfBlocks          _blocksHolder;
    BlockPtrsRing           _freeBlocks;
    BlockPtrsRing           _blocksQueue;
    Threads                 _threads;
    std::vector<size_t>     _counters;
    std::mutex              _blockMutex;
    std::mutex              _queueMutex;
    std::condition_variable _cvNonEmpty;
    std::condition_variable _cvNonFull;
    bool                    _stop = false;
};

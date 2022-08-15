
#include <cassert>
#include <algorithm>

#include "baseprodconsproc.h"

using namespace std;

static constexpr size_t BLOCK_SIZE = 4*1024;

BaseProdConsProcessor::BaseProdConsProcessor(size_t numOfConsThreads, size_t maxLines, size_t numOfBlocks):
_numOfThreads(numOfConsThreads + 1), _maxLines(maxLines),
_freeBlocks(numOfBlocks) {

    assert(numOfConsThreads > 0);
    assert(maxLines > 0);

    _blocksHolder.reserve(numOfBlocks);
    for(size_t i = 0; i < numOfBlocks; ++i) {
        LinesBlock blk;
        blk.lines.reserve(maxLines);
        _blocksHolder.push_back(std::move(blk));
    }

    _threads.resize(_numOfThreads);
}

BaseProdConsProcessor::~BaseProdConsProcessor() {
}

size_t BaseProdConsProcessor::execute(FileReader& freader, const string& filename,
                                WildcardMatch& wcmatch, const string& pattern) {

    doInit();

    if(freader.needsBuffer()) {
        // allocate memory for buffer
        for(auto& block: _blocksHolder) {
            block.buffer.resize(BLOCK_SIZE * _maxLines);
        }
    }

    ScopedFileOpener fopener(freader, filename);

    _threads[0] = thread(&BaseProdConsProcessor::readFileLines,
                                            this, std::ref(freader));

    for(size_t i = 1; i < _threads.size(); ++i) {
        _threads[i] = thread(&BaseProdConsProcessor::filterLines,
                            this, i-1, std::ref(wcmatch), pattern);
    }

    for(auto& t: _threads) {
        t.join();
    }

    return calcFinalResult();
}

BaseProdConsProcessor::LinesBlockPtr
BaseProdConsProcessor::readInLinesBlock(FileReader& freader) {

    const bool needsBuffer = freader.needsBuffer();

    LinesBlockPtr block = allocBlock();
    assert(block);
    auto &lines = block->lines;

    lines.clear();
    for(size_t i = 0; i < _maxLines; ++i) {
        if(needsBuffer) {
            auto* buffer = block->buffer.data();
            freader.setBuffer(buffer + i * BLOCK_SIZE, BLOCK_SIZE);
        }
        auto line = freader.readLine();
        if(!line.data()) {
            break;
        }
        lines.push_back(std::move(line));
    }

    return block;
}

void BaseProdConsProcessor::doInit() {

    init();

    // reset all pointers of free blocks
    _freeBlocks.clear();
    for(auto& block: _blocksHolder) {
        _freeBlocks.push(&block);
    }
}

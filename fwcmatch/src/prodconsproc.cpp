
#include <cassert>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <chrono>

#include "prodconsproc.h"

using namespace std;

static constexpr size_t BLOCK_SIZE = 4*1024;

ProdConsProcessor::ProdConsProcessor(size_t queueSize, size_t numOfConsThreads, size_t maxLines):
_blocksQueue(queueSize), _maxLines(maxLines) {

    assert(queueSize > 0);
    assert(numOfConsThreads > 0);
    assert(maxLines > 0);

    // +1 thread for a file reading
    auto numOfThreads = numOfConsThreads + 1;

    // for each block in queue and for each thread for waiting

    auto numOfBlocks = queueSize + numOfThreads;
    for(size_t i = 0; i < numOfBlocks; ++i) {
        LinesBlock blk;
        blk.lines.reserve(maxLines);
        _blocksHolder.push_back(std::move(blk));
    }

    _threads.resize(numOfThreads);
    _counters.resize(numOfConsThreads, 0);
}

size_t ProdConsProcessor::execute(FileReader& freader, const string& filename,
                                WildcardMatch& wcmatch, const string& pattern) {

    _stop = false;
    clear();

    if(freader.needsBuffer()) {
        for(auto& block: _blocksHolder) {
            block.buffer.resize(BLOCK_SIZE * _maxLines);
        }
    }

    ScopedFileOpener fopener(freader, filename);

    _threads[0] = thread(&ProdConsProcessor::readFileLines,
                                            this, std::ref(freader));

    for(size_t i = 1; i < _threads.size(); ++i) {
        _threads[i] = thread(&ProdConsProcessor::filterLines,
                            this, i-1, std::ref(wcmatch), pattern);
    }

    for(auto& t: _threads) {
        t.join();
    }

    size_t result = 0;
    for(auto c: _counters) {
        result += c;
    }
    return result;
}

void ProdConsProcessor::readFileLines(FileReader& freader) {

    const bool needsBuffer = freader.needsBuffer();

    size_t i;
    for(;;) {

        auto block = allocBlock();
        assert(block);
        auto &lines = block->lines;

        lines.clear();
        for(i = 0; i < _maxLines; ++i) {
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

        if(lines.empty()) {
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

void ProdConsProcessor::filterLines(size_t idx,
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

        _cvNonFull.notify_one();
        lock.unlock();

        assert(block);

        auto& lines = block->lines;
        counter += count_if(lines.cbegin(), lines.cend(),
            [&](auto const& line){ return wcmatch.isMatch(line, pattern); }
        );

        lines.clear();
        freeBlock(block);
    }

    _counters[idx] = counter;
}

ProdConsProcessor::LinesBlockPtr ProdConsProcessor::allocBlock() {
    scoped_lock lock(_blockMutex);
    if(_freeBlocks.empty()) {
        return nullptr;
    }

    auto p = _freeBlocks.top();
    _freeBlocks.pop();
    return p;
}

void ProdConsProcessor::freeBlock(ProdConsProcessor::LinesBlockPtr p) {
    scoped_lock lock(_blockMutex);
    _freeBlocks.push(p);
}

void ProdConsProcessor::clear() {

    _freeBlocks = {};
    for(int i = _blocksHolder.size() - 1; i >= 0; --i) {
        _freeBlocks.push(&_blocksHolder[i]);
    }

    _blocksQueue.clear();

    std::memset(_counters.data(), 0, _counters.size() * sizeof(_counters[0]));
}
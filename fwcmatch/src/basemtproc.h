#pragma once

#include <cstddef>
#include <string>
#include <numeric>
#include <algorithm>
#include <vector>
#include <thread>

#include "noncopyable.h"
#include "linesblock.h"
#include "wildcard.h"
#include "filereader.h"

class BaseMTProcessor: private noncopyable
{
public:

    BaseMTProcessor(size_t numOfConsThreads, size_t maxLines, bool needsBuffer);
    virtual ~BaseMTProcessor();

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

protected:

    // must be used in readFileLines
    void readInLinesBlock(FileReader& freader, LinesBlock& block);

    // must be used in filterLines
    size_t filterBlock(WildcardMatch& wcmatch, const std::string& pattern,
                                                        LinesBlock const& block);

    void initLinesBlock(LinesBlock& block,
                    const size_t blockSize = BlocksBuffer::DEFAULT_BLOCK_SIZE);

    std::vector<size_t> _counters;
    const size_t        _numOfConsThreads;
    const size_t        _maxLines;
    const bool          _needsBuffer;

private:

    // it is called in the 'execute' method in the beginning (so threads haven't started yet)
    virtual void init() = 0;

    // it is called in producer thread
    virtual void readFileLines(FileReader& freader) = 0;

    // it is called in consumer threads
    virtual void filterLines(size_t idx, WildcardMatch& wcmatch, const std::string& pattern) = 0;

    // gather all counters from all consumers
    // it is called in the 'execute' method after all threads finished
    virtual size_t calcFinalResult() const;
};

inline void BaseMTProcessor::readInLinesBlock(FileReader& freader, LinesBlock& block) {

    const bool needsBuffer = freader.needsBuffer();

    auto& lines  = block.lines;
    auto& buffer = block.buffer;
    auto* bufferPtr = needsBuffer ? buffer.get(0) : nullptr;
    size_t lastLineSize = 0;

    lines.clear();
    for(size_t i = 0; i < _maxLines; ++i) {
        if(needsBuffer) {
            //freader.setBuffer(buffer.get(i), buffer.blockSize());

            // this way makes memory locality much better and improve performance
            bufferPtr += lastLineSize;
            freader.setBuffer(bufferPtr, buffer.blockSize());
        }
        auto line = freader.readLine();
        if(!line.data()) {
            break;
        }
        lastLineSize = line.size();
        lines.push_back(std::move(line));
    }
}

inline size_t BaseMTProcessor::filterBlock(WildcardMatch& wcmatch,
                                    const std::string& pattern, LinesBlock const& block) {

    auto& lines = block.lines;
    size_t counter = count_if(lines.cbegin(), lines.cend(),
        [&](auto const& line){ return wcmatch.isMatch(line, pattern); }
    );

    return counter;
}

inline void BaseMTProcessor::initLinesBlock(LinesBlock& block, const size_t blockSize) {
    block.lines.reserve(_maxLines);
    if(_needsBuffer) {
        block.buffer.resize(_maxLines, blockSize);
    }
}

inline size_t BaseMTProcessor::calcFinalResult() const {

    return std::accumulate(_counters.begin(), _counters.end(),
                                decltype(_counters)::value_type(0));
}

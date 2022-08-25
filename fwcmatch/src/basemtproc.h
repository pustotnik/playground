#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <thread>

#include "noncopyable.h"
#include "linesblock.h"
#include "ringbuffer.h"
#include "wildcard.h"
#include "filereader.h"

class BaseMTProcessor: private noncopyable
{
public:

    BaseMTProcessor(size_t numOfConsThreads, size_t maxLines);
    virtual ~BaseMTProcessor();

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

protected:

    // must be used in readFileLines
    void readInLinesBlock(FileReader& freader, LinesBlock& block);

    // must be used in filterLines
    size_t filterBlock(WildcardMatch& wcmatch, const std::string& pattern,
                                                            LinesBlock& block);

private:
    using Threads = std::vector<std::thread>;

    // it is called in the 'execute' method in the beginning (so threads haven't started yet)
    virtual void init() = 0;

    // it is called in producer thread
    virtual void readFileLines(FileReader& freader) = 0;

    // it is called in consumer threads
    virtual void filterLines(size_t idx, WildcardMatch& wcmatch, const std::string& pattern) = 0;

    // gather all counters from all consumers
    // it is called in the 'execute' method after all threads finished
    virtual size_t calcFinalResult() const = 0;

    const size_t _maxLines;
    Threads      _threads;
};

inline void BaseMTProcessor::readInLinesBlock(FileReader& freader, LinesBlock& block) {

    const bool needsBuffer = freader.needsBuffer();

    auto& lines  = block.lines;
    auto& buffer = block.buffer;

    lines.clear();
    for(size_t i = 0; i < _maxLines; ++i) {
        if(needsBuffer) {
            freader.setBuffer(buffer.get(i), buffer.blockSize());
        }
        auto line = freader.readLine();
        if(!line.data()) {
            break;
        }
        lines.push_back(std::move(line));
    }
}

inline size_t BaseMTProcessor::filterBlock(WildcardMatch& wcmatch,
                                        const std::string& pattern, LinesBlock& block) {

    auto& lines = block.lines;
    size_t counter = count_if(lines.cbegin(), lines.cend(),
        [&](auto const& line){ return wcmatch.isMatch(line, pattern); }
    );

    lines.clear();

    return counter;
}

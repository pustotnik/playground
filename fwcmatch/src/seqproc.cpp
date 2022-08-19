
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <iostream>

#include "seqproc.h"

using namespace std;

static constexpr size_t BLOCK_SIZE = 4*1024;

SequentialProcessor::SequentialProcessor(const size_t maxLines):
    _maxLines(maxLines) {

    assert(_maxLines > 0);
    _linesBlock.buffer.resize(maxLines, BLOCK_SIZE);
    _linesBlock.lines.reserve(_maxLines);
}

size_t SequentialProcessor::execute(FileReader& freader, const string& filename,
                            WildcardMatch& wcmatch, const string& pattern) {

    const bool needsBuffer = freader.needsBuffer();
    auto& buffer = _linesBlock.buffer;
    auto& flines = _linesBlock.lines;

    ScopedFileOpener fopener(freader, filename);

    size_t i, result = 0;
    for(;;) {
        flines.clear();
        for(i = 0; i < _maxLines; ++i) {
            if(needsBuffer) {
                freader.setBuffer(buffer.get(i), buffer.blockSize());
            }
            auto line = freader.readLine();
            if(!line.data()) {
                break;
            }
            flines.emplace_back(std::move(line));
        }

        if(flines.empty()) {
            break;
        }

        result += std::count_if(flines.begin(), flines.end(),
            [&](auto& line){ return wcmatch.isMatch(line, pattern); }
        );
    }

    return result;
}
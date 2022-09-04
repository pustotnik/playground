
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <iostream>

#include "seqproc.h"

using namespace std;

static constexpr size_t BLOCK_SIZE = 4*1024;

SequentialProcessor::SequentialProcessor(size_t maxLines, bool needsBuffer):
    _maxLines(maxLines) {

    assert(_maxLines > 0);
    _linesBlock.alloc(maxLines, needsBuffer, BLOCK_SIZE);
}

size_t SequentialProcessor::execute(FileReader& freader, const string& filename,
                            WildcardMatch& wcmatch, const string& pattern) {

    const bool needsBuffer = freader.needsBuffer();
    auto& buffer = _linesBlock.buffer();

    ScopedFileOpener fopener(freader, filename);

    size_t i, result = 0;
    for(;;) {
        _linesBlock.clear();
        for(i = 0; i < _maxLines; ++i) {
            if(needsBuffer) {
                freader.setBuffer(buffer.get(i), buffer.blockSize());
            }
            auto line = freader.readLine();
            if(!line.data()) {
                break;
            }
            _linesBlock.addLine(std::move(line));
        }

        auto const& flines = _linesBlock.lines();
        if(flines.empty()) {
            break;
        }

        result += std::count_if(flines.begin(), flines.end(),
            [&](auto& line){ return wcmatch.isMatch(line, pattern); }
        );
    }

    return result;
}
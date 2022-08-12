
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <iostream>

#include "seqproc.h"

using namespace std;

static constexpr size_t BLOCK_SIZE = 4*1024;

SequentialProcessor::SequentialProcessor(size_t maxLines):
_maxLines(maxLines), _buffer(BLOCK_SIZE * maxLines) {
    assert(_maxLines > 0);
    _flines.reserve(_maxLines);
}

size_t SequentialProcessor::execute(FileReader& freader, const string& filename,
                            WildcardMatch& wcmatch, const string& pattern) {

    ScopedFileOpener fopener(freader, filename);

    char* buffer = _buffer.data();
    const bool needsBuffer = freader.needsBuffer();

    size_t i, result = 0;
    for(;;) {
        _flines.clear();
        for(i = 0; i < _maxLines; ++i) {
            if(needsBuffer) {
                freader.setBuffer(buffer + i * BLOCK_SIZE, BLOCK_SIZE);
            }
            auto line = freader.readLine();
            if(!line.data()) {
                break;
            }
            _flines.emplace_back(std::move(line));
        }

        if(_flines.empty()) {
            break;
        }

        result += std::count_if(_flines.begin(), _flines.end(),
            [&](auto& line){ return wcmatch.isMatch(line, pattern); }
        );
    }

    return result;
}
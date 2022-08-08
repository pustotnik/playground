
#include <cassert>
#include <cstddef>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>

#include "seqproc.h"

using namespace std;

static constexpr size_t BLOCK_SIZE = 4*1024;

size_t SequentialProcessor::execute(const std::string& filename, const std::string& pattern,
                WildcardMatch& wcmatch, FileReader& freader, size_t maxLines) {

    assert(maxLines > 0);

    freader.open(filename);

    vector<char> bufferData(BLOCK_SIZE * maxLines);
    char* buffer = bufferData.data();

    vector<string_view> lines;
    lines.reserve(maxLines);
    const bool needsBuffer = freader.needsBuffer();

    size_t i, result = 0;
    for(;;) {
        lines.clear();
        for(i = 0; i < maxLines; ++i) {
            if(needsBuffer) {
                freader.setBuffer(buffer + i * BLOCK_SIZE, BLOCK_SIZE);
            }
            auto line = freader.readLine();
            if(!line.data()) {
                break;
            }
            lines.emplace_back(line);
        }

        if(lines.empty()) {
            break;
        }

        result += std::count_if(lines.begin(), lines.end(),
            [&](auto& line){ return wcmatch.isMatch(line, pattern); }
        );
    }

    freader.close();

    return result;
}
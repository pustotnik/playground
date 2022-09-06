
#include <cassert>

#include "proctools.h"

namespace fwc {
namespace proctools {

void readInLinesBlock(FileReader& freader, LinesBlock& block) {

    const bool needsBuffer = freader.needsBuffer();
    const auto maxLines = block.maxLines();

    auto& buffer = block.buffer();
    auto* bufferPtr = needsBuffer ? buffer.get(0) : nullptr;
    size_t lastLineSize = 0;

    block.clear();
    for(size_t i = 0; i < maxLines; ++i) {
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
        block.addLine(std::move(line));
    }
}

} // namespace proctools
} // namespace fwc
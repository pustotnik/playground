#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "noncopyable.h"
#include "linesblock.h"
#include "wildcard.h"
#include "filereader.h"

namespace fwc {

class SequentialProcessor final: private noncopyable
{
public:

    SequentialProcessor(size_t maxLines, bool needsBuffer);

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

private:

    LinesBlock   _linesBlock;
};

} // namespace fwc
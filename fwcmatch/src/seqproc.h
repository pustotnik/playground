#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "wildcard.h"
#include "filereader.h"

class SequentialProcessor final
{
public:

    size_t execute(const std::string& filename, const std::string& pattern,
                WildcardMatch& wcmatch, FileReader& freader, size_t maxLines);
};

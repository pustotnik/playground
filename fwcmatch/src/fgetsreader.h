#pragma once

#include <cstddef>
#include <cstdio>

#include "filereader.h"

namespace fwc {

class FGetsReader final: public FileReader
{
public:

    ~FGetsReader();

    // open file
    void open(const std::string& filename) override;

    // close file
    void close() override;

    bool needsBuffer() const override { return true; } ;

    // read next line in file
    // FileLineRef is used to avoid copying
    FileLineRef readLine() override;

private:
    FILE*  _file { nullptr };
};

} // namespace fwc

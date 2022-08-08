#pragma once

#include <cstddef>
#include <cstdio>

#include "filereader.h"

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
    // string_view is used to avoid copying
    std::string_view readLine() override;

private:
    FILE*  _file = nullptr;
};

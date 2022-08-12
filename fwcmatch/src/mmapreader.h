#pragma once

#include <cstddef>
#include <cstdio>

#include "filereader.h"

class MMapReader final: public FileReader
{
public:

    ~MMapReader();

    // open file
    void open(const std::string& filename) override;

    // close file
    void close() override;

    bool needsBuffer() const override { return false; } ;

    // read next line in file
    // FileLineRef is used to avoid copying
    FileLineRef readLine() override;

private:
    void*       _addr = nullptr;
    const char* _mapptr = nullptr;
    const char* _mapend = nullptr;
    size_t      _fileSize = 0;
    int         _file = -1;
};

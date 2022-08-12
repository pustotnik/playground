#pragma once

#include <fstream>

#include "filereader.h"

class FStreamReader final: public FileReader
{
public:

    ~FStreamReader();

    // open file
    void open(const std::string& filename) override;

    // close file
    void close() override;

    bool needsBuffer() const override { return true; } ;

    // read next line in file
    // FileLineRef is used to avoid copying
    FileLineRef readLine() override;

private:
    std::ifstream _stream;
};

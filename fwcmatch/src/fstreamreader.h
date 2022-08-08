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
    // string_view is used to avoid copying
    std::string_view readLine() override;

private:
    std::ifstream _stream;
};

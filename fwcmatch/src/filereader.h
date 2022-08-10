#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "noncopyable.h"

class FileReader: private noncopyable
{
public:
    virtual ~FileReader() {};

    // open file
    virtual void open(const std::string& filename) = 0;

    // close file
    virtual void close() = 0;

    virtual bool needsBuffer() const = 0;

    void setBuffer(char* buffer, size_t bufferSize);

    // read next line in file
    // string_view is used to avoid copying
    virtual std::string_view readLine() = 0;

protected:
    char*  _buffer = nullptr;
    size_t _bufferSize = 0;
};

inline void FileReader::setBuffer(char* buffer, size_t bufferSize) {
    _buffer = buffer;
    _bufferSize = bufferSize;
}

// RAII for open/close of FileReader
class ScopedFileOpener final: private noncopyable {
public:
    ScopedFileOpener(FileReader& freader, const std::string& filename):
    _freader(freader) {
        _freader.open(filename);
    }

    ~ScopedFileOpener() {
        _freader.close();
    }

private:
    FileReader& _freader;
};
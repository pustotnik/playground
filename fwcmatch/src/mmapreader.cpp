
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "mmapreader.h"
#include "common.h"

MMapReader::~MMapReader() {
    close();
}

// open file
void MMapReader::open(const std::string& filename) {
    if(filename.empty()) {
        errorAndStop("File name is empty", false);
    }

    // I'm not sure that file descriptor can be zero but manual (man) says it's
    // a nonnegative integer.
    if(_file >= 0) {
        // file is open
        return;
    }

    _file = ::open(filename.c_str(), O_RDONLY);
    if(-1 == _file) {
        errorAndStop("File opening failed");
    }

    // obtain file size
    struct stat sb;
    if (::fstat(_file, &sb) == -1) {
        errorAndStop("fstat");
    }

    _fileSize = sb.st_size;

    _addr = ::mmap(NULL, _fileSize, PROT_READ, MAP_PRIVATE|MAP_POPULATE, _file, 0u);
    if(MAP_FAILED == _addr) {
        errorAndStop("mmap");
    }

    _mapptr = static_cast<const char*>(_addr);
    _mapend = _mapptr + _fileSize;
}

// close file
void MMapReader::close() {

    if(_addr) {
        ::munmap(_addr, _fileSize);
        _addr = nullptr;
        _mapptr = _mapend = nullptr;
    }

    if(_file >= 0) {
        ::close(_file);
        _file = -1;
    }
}

// read next line in file
std::string_view MMapReader::readLine() {

    // It is experimental code and so I don't do correct error handling for all cases
    assert(_file >= 0);
    assert(_mapptr);

    if( _mapptr >= _mapend) {
        return {};
    }

    size_t lineSize = 0;

    // strip newline symbol
    const char* eol = static_cast<const char*>(
            ::memchr(_mapptr, '\n', _mapend - _mapptr));

    size_t eolOffset = 0;
    if(eol) {
        eolOffset = 1;
        if(eol != _mapptr && *(eol-1) == '\r') {
            --eol;
            ++eolOffset;
        }
        lineSize = eol - _mapptr;
    }
    else {
        lineSize = _mapend - _mapptr;
    }

    std::string_view result { _mapptr, lineSize };
    _mapptr += lineSize + eolOffset;

    return result;
}


#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>

#include "utils.h"
#include "fstreamreader.h"

namespace fwc {

FStreamReader::~FStreamReader() {
    close();
}

// open file
void FStreamReader::open(const std::string& filename) {
    if(filename.empty()) {
        errorAndStop("File name is empty", false);
    }

    if(_stream.is_open()) {
        return;
    }

    _stream.open(filename);
    if(!_stream) {
        errorAndStop("File opening failed", false);
    }
}

// close file
void FStreamReader::close() {
    if(_stream.is_open()) {
        _stream.close();
    }
}

// read next line in file
FileLineRef FStreamReader::readLine() {

    // It is experimental code and so I don't do correct error handling for all cases
    assert(_stream.is_open());
    assert(_buffer && _bufferSize > 1);

    // I don't use std::getline because it cannot be used with char* buffer
    _stream.getline(_buffer, _bufferSize);

    if (_stream.bad()) {
        errorAndStop("I/O error while reading", false);
    }

    if(_stream.eof()) {
        return {};
    }

    size_t lineSize = _stream.gcount();
    if(!lineSize) {
        errorAndStop("Logical error while reading", false);
    }

    if(!_stream.fail()) {
        // basic_istream::getline set failbit if a delimiter was not found
        // So here we found the delimiter '\n'

        --lineSize;
        const char* eol = _buffer + lineSize;
        if(eol != _buffer && *(eol-1) == '\r') {
            --lineSize;
        }
    }

    return { _buffer, lineSize };
}

} // namespace fwc
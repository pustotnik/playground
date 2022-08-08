
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>

#include "fgetsreader.h"
#include "common.h"

FGetsReader::~FGetsReader() {
    close();
}

// open file
void FGetsReader::open(const std::string& filename) {
    if(filename.empty()) {
        errorAndStop("File name is empty", false);
    }

    if(_file) {
        // file is open
        return;
    }

    _file = fopen(filename.c_str(), "rb");
    if(!_file) {
        errorAndStop("File opening failed");
    }

    //setvbuf(_file , NULL , _IOFBF , 1024*4);
}

// close file
void FGetsReader::close() {
    if(_file) {
        fclose(_file);
        _file = nullptr;
    }
}

// read next line in file
std::string_view FGetsReader::readLine() {

    // It is experimental code and so I don't do correct error handling for all cases
    assert(_file);
    assert(_buffer && _bufferSize > 1);

    if( !fgets(_buffer, _bufferSize, _file)) {
        return {};
    }

    // strip newline symbol
    size_t lineSize = 0;
    const char* eol = strchr(_buffer, '\n');
    if(eol) {
        if(eol != _buffer && *(eol-1) == '\r') {
            --eol;
        }
        lineSize = eol - _buffer;
    }
    else {
        lineSize = strnlen(_buffer, _bufferSize);
    }

    return { _buffer, lineSize };
}

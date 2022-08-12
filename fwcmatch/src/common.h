#pragma once

#include <cstdio>
#include <cerrno>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

typedef std::string_view         FileLineRef;
typedef std::vector<FileLineRef> FileLineRefs;

[[ noreturn ]]
inline void errorAndStop(const std::string& msg, bool useErrno = true) {
    if(useErrno) {
        perror(msg.c_str());
    }
    else {
        std::cerr << msg << std::endl;
    }
    exit(255);
}

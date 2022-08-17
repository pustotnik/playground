#pragma once

#include <cstdio>
#include <cerrno>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

using FileLineRef  = std::string_view;
using FileLineRefs = std::vector<FileLineRef>;

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

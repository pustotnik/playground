#pragma once

#include <cstdio>
#include <cerrno>
#include <string>
#include <vector>
#include <iostream>

typedef std::vector<std::string> Strings;

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

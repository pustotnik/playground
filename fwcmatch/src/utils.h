#pragma once

#include <cstdio>
#include <cerrno>
#include <string>
#include <iostream>

namespace fwc {

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

} // namespace fwc
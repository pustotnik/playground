
###############################################################
## HINTS:

## Configure the build
# cmake -S . -B build
# cmake -S . -B build/release -D CMAKE_BUILD_TYPE=Release
## Actually build the binaries
# cmake --build build
# cmake --build build/release

# (cd build/; make clean; cmake --build . -v)
# (cd build/release; make clean; cmake --build . -v)

###############################################################

MESSAGE(STATUS "Running cmake version ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 3.10)
project(FWCMatch VERSION 0.1 LANGUAGES CXX)

# Disable in-source builds to prevent source tree corruption.
# https://github.com/opencv/opencv/blob/master/CMakeLists.txt
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMN_GCC_FLAGS -Wall -Wextra -std=c++20 -fno-rtti)

set(CMN_GCC_RELEASE_OPTS  ${CMN_GCC_FLAGS} -O3)
set(CMN_GCC_DEBUG_OPTS    ${CMN_GCC_FLAGS} -O0)

# gcc/clang
add_compile_options(
  "$<$<CONFIG:DEBUG>:${CMN_GCC_DEBUG_OPTS}>"
  "$<$<CONFIG:RELEASE>:${CMN_GCC_RELEASE_OPTS}>"
)

###########################################################################

find_package(benchmark REQUIRED)

set(SRC_LIST
    src/bench.cpp
    src/basemtproc.cpp
    src/fgetsreader.cpp
    src/fnmatchwildcard.cpp
    src/fstreamreader.cpp
    src/mmapreader.cpp
    src/mtcondvarproc.cpp
    src/mtcondvarproc2.cpp
    src/mtlockfreeproc.cpp
    src/mtsemproc.cpp
    src/mywildcard.cpp
    src/regexwildcard.cpp
    src/seqproc.cpp)

add_executable(fwcmatch-bench ${SRC_LIST})
target_link_libraries(fwcmatch-bench benchmark::benchmark)


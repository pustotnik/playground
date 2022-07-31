
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
project(CpuCache VERSION 0.1 LANGUAGES CXX)

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

set(CMN_GCC_FLAGS -Wall -Wextra -std=c++17 -fno-exceptions -fno-rtti)

#set(CMN_GCC_RELEASE_OPTS  ${CMN_GCC_FLAGS} -O2)
set(CMN_GCC_RELEASE_OPTS  ${CMN_GCC_FLAGS} -O3)
set(CMN_GCC_DEBUG_OPTS    ${CMN_GCC_FLAGS} -O0)
set(CMN_MSVC_RELEASE_OPTS /O2 /EHsc /std:c++17)
set(CMN_MSVC_DEBUG_OPTS   /Od /EHsc /std:c++17)

if(MSVC)
    add_compile_options(
      "$<$<CONFIG:DEBUG>:${CMN_MSVC_DEBUG_OPTS}>"
      "$<$<CONFIG:RELEASE>:${CMN_MSVC_RELEASE_OPTS}>"
    )
else() # gcc/clang
    add_compile_options(
      "$<$<CONFIG:DEBUG>:${CMN_GCC_DEBUG_OPTS}>"
      "$<$<CONFIG:RELEASE>:${CMN_GCC_RELEASE_OPTS}>"
    )
endif()

###########################################################################

find_package(benchmark REQUIRED)

add_executable(traversals traversals.cpp)
target_link_libraries(traversals benchmark::benchmark)

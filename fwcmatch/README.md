
## Why

I wanted to make some benchmarks to see some real performance on some real
task with different solutions and different parameters and methods.

## The task

We read one some big (it is important) log file line by line then filter these
lines with some wildcard pattern and count all filtered lines.
One detail of this task is necessity to read file line by line, not block by block.
So we cannot know where a line is ended before reading that line. As result we
cannot read such a file in more than one thread if we're reading this file the first time.

I decided to use at least three methods for a file reading:
C/C++ **fgets**, C++ **iostream** and Linux **mmap**.
For wildcard matching I used my general implementation of
the wildcard matching algorithm based on this [solution](https://yucoding.blogspot.com/2013/02/leetcode-question-123-wildcard-matching.html) and the POSIX fnmatch function.
Also I implemented wildcard matching with
C++ standard regex library but it works really slow.
But I had no purpose to benchmark wildcard algorithms.
And I implemented this as a single-threaded and as a multi-threaded solutions.
Also I wanted to play with such paramenters as number of read/filtered
lines at one time and number of threads.

*NOTE*: I wrote this code **avoiding any memory (re)allocation on heap during reading and
filtering**. All needed memory is allocated beforehand. For example for queue to
communicate between producer and consumers in multi-threaded implementation
a fixed size ring/circular buffer is used.

This code is for Linix OS only. Maybe it can work on MacOS but I haven't tried.

To build and run I used my build system like this (from directory playground/fwcmatch):
```
BENCH_FILENAME="/files/tmp/_unison.log" BENCH_PATTERN="*failed*" zenmake run
```

But CMakeLists.txt for cmake build system is also provided.

## The results

#### Testing environment
- HP Elitebook 850 g5
- CPU: Intel(R) Core(TM) i5-8250U (4 cores 8 threads)
- Memory: 16GB
- Data storage: NVMe SSD
- OS: Gentoo Linux (kernel 5.10.27)
- Filesystem: ext4
- C++ compiler: g++ 11.2.1 (-std=c++20 -O3 -fno-rtti)
- CPU Frequency Scaling is turned off:
  ```
  sudo cpupower frequency-set --governor performance
  ```
- CPU Frequency is limited to 3000MHz (it has nothing to do with these benchmarks).
- CPU Caches:
  - L1 Data 32 KiB (x4)
  - L1 Instruction 32 KiB (x4)
  - L2 Unified 256 KiB (x4)
  - L3 Unified 6144 KiB (x1)

#### Output
I created a 4GB file from my own unison.log and used the
wildcard pattern \*failed\* for all benchmarks.

Some fields were cut off for shortance.

```
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                                   Time             CPU
----------------------------------------------------------------------------------------------------------------------------------

===== Single-threaded solution

BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:1/real_time                                           9013 ms         9013 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:4/real_time                                           9271 ms         9271 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:20/real_time                                          9285 ms         9285 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:40/real_time                                          9398 ms         9398 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:1/real_time                                         8697 ms         8697 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:4/real_time                                         8799 ms         8799 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:20/real_time                                        8724 ms         8724 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:40/real_time                                        8663 ms         8662 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:1/real_time                                            7085 ms         7085 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:4/real_time                                            7211 ms         7210 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:20/real_time                                           7217 ms         7216 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:40/real_time                                           7140 ms         7139 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:1/real_time                                                  11217 ms        11216 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:4/real_time                                                  11534 ms        11533 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:20/real_time                                                 11468 ms        11467 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:40/real_time                                                 11546 ms        11545 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:1/real_time                                                10911 ms        10910 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:4/real_time                                                11070 ms        11070 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:20/real_time                                               11048 ms        11047 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:40/real_time                                               10869 ms        10869 ms
BM_Sequential<MMapReader, FNMatch>/mlines:1/real_time                                                    9278 ms         9278 ms
BM_Sequential<MMapReader, FNMatch>/mlines:4/real_time                                                    9425 ms         9424 ms
BM_Sequential<MMapReader, FNMatch>/mlines:20/real_time                                                   9324 ms         9321 ms
BM_Sequential<MMapReader, FNMatch>/mlines:40/real_time                                                   9344 ms         9336 ms

===== Multi-threaded solution as single producer and many consumers with conditional variables

BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            9686 ms        16864 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           6909 ms        11633 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            7674 ms        21127 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           4719 ms        12735 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           4883 ms        14624 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           5042 ms        15009 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           5927 ms        16364 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           5812 ms        16263 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          5821 ms        16402 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           6052 ms        15493 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           6224 ms        18263 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time         10053 ms        16641 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time         6706 ms        10089 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time          6663 ms        17802 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time         3419 ms         9725 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time         3563 ms        12383 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time         3498 ms        12176 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time         3713 ms        13454 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time         3805 ms        13870 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time        3871 ms        14062 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time         4368 ms        14762 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time         4364 ms        17020 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time             9750 ms        15113 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time            6957 ms         8417 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time             7088 ms        17338 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time            3752 ms         8468 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time            2952 ms         9999 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time            2712 ms         8902 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time            2533 ms         8125 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time            2594 ms         8239 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time           2569 ms         8190 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time            1977 ms        13201 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time            2044 ms        13927 ms

===== Multi-threaded solution as single producer and many consumers with conditional variables (Ver 2)

BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time           9546 ms        16585 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time          6806 ms        11375 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time           7196 ms        19896 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time          4594 ms        12421 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time          4952 ms        14619 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time          4791 ms        14665 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time          5497 ms        14557 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time          5406 ms        14161 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time         5436 ms        14217 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time          5617 ms        14416 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time          5502 ms        16172 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time         9916 ms        16447 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time        6805 ms        10218 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time         6560 ms        17558 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time        3417 ms         9728 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time        3553 ms        12399 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time        3517 ms        12647 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time        3311 ms        11263 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time        3660 ms        12564 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time       3741 ms        12875 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time        4133 ms        13854 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        4002 ms        14067 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            9607 ms        14858 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           6799 ms         8234 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            6925 ms        17061 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           3585 ms         8056 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           2884 ms         9723 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           2544 ms         8226 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           2441 ms         7793 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           2448 ms         7769 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          2436 ms         7746 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           1912 ms        12900 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           2030 ms        13718 ms

===== Multi-threaded solution as single producer and many consumers without locks

BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time           7778 ms        15555 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time          6801 ms        13600 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time           5096 ms        15286 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time          4551 ms        13651 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time          4539 ms        18151 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time          6207 ms        24817 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time          6470 ms        25818 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time          6437 ms        25637 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time         6516 ms        25869 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time         10267 ms        81453 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time         10703 ms        84283 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time         7571 ms        15142 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time        6396 ms        12791 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time         4099 ms        12296 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time        3313 ms         9937 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time        3052 ms        12204 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time        3761 ms        15034 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time        3840 ms        15292 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time        3970 ms        15767 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time       4038 ms        15955 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time        7340 ms        58005 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        7232 ms        56745 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            7162 ms        14072 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           6580 ms        12914 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            4078 ms        11739 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           3490 ms         9986 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           2479 ms         9190 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           2475 ms         9175 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           2555 ms         9482 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           2707 ms        10087 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          2708 ms        10092 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           2259 ms        16261 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           2245 ms        16150 ms

===== Multi-threaded solution as single producer and many consumers with semaphores (since C++20)

BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time               10685 ms        21345 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time               7418 ms        13651 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time                8139 ms        23312 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time               4862 ms        13698 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time               5019 ms        17191 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time               5021 ms        17302 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time               6444 ms        17387 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time               6253 ms        17012 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time              6300 ms        17251 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time               7240 ms        24789 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time               6839 ms        20195 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time             10539 ms        21033 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time             7214 ms        12342 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time              6729 ms        19094 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time             3442 ms        10192 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time             3557 ms        13276 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time             3539 ms        13162 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time             3889 ms        14014 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time             3927 ms        14246 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time            3924 ms        14346 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time             5575 ms        24290 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time             4756 ms        18634 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time                10084 ms        19873 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time                7154 ms        10204 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time                 5376 ms        15638 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time                3645 ms         9135 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time                2660 ms         9469 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time                2504 ms         8717 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time                2470 ms         8137 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time                2409 ms         7875 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time               2513 ms         8323 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time                2247 ms        15476 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time                2199 ms        15156 ms

```

- BM_Sequential   - Single-threaded implementation
- BM_MTCondVar    - Multi-threaded implementation as a Producer-Consumer solution using
                    mutex and condition variables.
- BM_MTCondVar2   - The same as BM_MTCondVar but uses only one mutex and less locks.
- BM_MTLockFree   - Multi-threaded implementation as a Producer-Consumer solution
                    with atomic variables without any locks (busy-waiting).
- BM_MTSem        - Multi-threaded implementation as a Producer-Consumer solution using
                    mutex and semaphores.
- FGetsReader     - The fgets is used
- FStreamReader   - The iostream is used
- MMapReader      - The mmap is ised
- MyWildcardMatch - Manual implementation of wildcard matching algorithm
- FNMatch         - The fnmatch is used
- mlines          - Max number of file lines read/filtered lines at one time
- qsize           - Size of queue for multi-threaded implementations.
                    For BM_MTLockFree it means the size of a queue for each consumer.
- threads         - Number of threads for multi-threaded implementation
- column 'CPU' means sum of time from all used CPUs.

To reduce size of the report I exclused the use of FNMatch from multi-threaded benchmarks.

## Some observations
All next observations are actual only for **these** benchmarks on **this** system.
On different OS/hardware/implementation the results can be different.

- C++ iostream can work a little bit faster then C fgets.
- FNMatch works a little bit slower than MyWildcardMatch just because fnmatch
  requires C string terminated with '\0' and I had to make copy of each file
  line because it is requirement of my interface (string_view is used).
- There is no reason to use number of read/filtered lines at one time more than
  one for single-threaded implementation.
  But it is important for multi-threaded implementation.
- It is often optimal to set size of queue to number of threads.
- Increasing number of threads in multi-threaded implementation is not always
  optimal decision. But it is because file I/O is bottleneck for this task.
- Lock free solution is not the best choice for a common case.
- MTSem (semaphores) is very similar to the MTCondVar (condition variables)
  in terms of performance but the MTCondVar is better.
- BM_MTCondVar2 is a little bit faster than BM_MTCondVar but there no big difference.

## About mmap
The **mmap** in these benchmarks are fastest but you should to know that it was warm run and the
input file could fit in the memory. For example if you clear pagecache with this command:
```
sync; echo 1 > /proc/sys/vm/drop_caches
```
then the first run of any test with mmap can show noticeable regression:

```
-----------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                               Time             CPU
-----------------------------------------------------------------------------------------------------------------------------

=== Warm buffer cache:

BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        1885 ms         12674 ms

=== Cold buffer cache (after `sync; echo 1 > /proc/sys/vm/drop_caches`):

BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        9776 ms         15124 ms
```

As I understand the mmap is a good method to read the same file several times and
for a random access (especially on SSD).
Also I can recommend to read [this](https://sasha-f.medium.com/why-mmap-is-faster-than-system-calls-24718e75ab37).

## About lock-free solution
In this solution I used wait-free and lock free implementation of
ring buffer from https://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular.
This container is thread-safe but only for single producer and single consumer variant.
So I implemented this as a group of pairs "single producer -> single consumer" where
my single producer emulates many producers in a loop. In some cases this solution showed better
performance than solution with conditional variables but not always.
The main problem with this is busy-waiting on CPU when these queues are full or empty.
Look at the column "CPU" to see the proof. And if you set number of threads
more than necessery for optimal processing then a lot of time this code will
just "burn" CPU cores what is not good at all.
So be careful with such lock-free solutions. You should understand what you are doing.
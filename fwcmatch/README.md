
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

NOTE: I wrote this code avoiding any memory reallocation during reading and
filtering. For example for queue in multi-threaded implementation a simple
fixed size ring/circular buffer is used.
And all memory buffers are allocated beforehand.

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
- C++ compiler: g++ 11.2.1 (-std=c++20 -O3 -fno-rtti -flto)
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
I created a 2GB file from my own unison.log and used the
wildcard pattern \*failed\* for all benchmarks.

Some fields were cut off for shortance.

```
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                                   Time             CPU
----------------------------------------------------------------------------------------------------------------------------------

===== Single-threaded solution

BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:1/real_time                                           5160 ms         5158 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:2/real_time                                           5322 ms         5320 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:4/real_time                                           5371 ms         5369 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:20/real_time                                          5342 ms         5341 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:40/real_time                                          5321 ms         5321 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:1/real_time                                         3611 ms         3611 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:2/real_time                                         3680 ms         3680 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:4/real_time                                         3703 ms         3703 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:20/real_time                                        3636 ms         3636 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:40/real_time                                        3629 ms         3629 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:1/real_time                                            3555 ms         3555 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:2/real_time                                            3646 ms         3646 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:4/real_time                                            3675 ms         3675 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:20/real_time                                           3664 ms         3664 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:40/real_time                                           3624 ms         3624 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:1/real_time                                                   5617 ms         5617 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:2/real_time                                                   5804 ms         5804 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:4/real_time                                                   5812 ms         5812 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:20/real_time                                                  5731 ms         5731 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:40/real_time                                                  5752 ms         5752 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:1/real_time                                                 5432 ms         5432 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:2/real_time                                                 5499 ms         5499 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:4/real_time                                                 5452 ms         5452 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:20/real_time                                                5404 ms         5404 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:40/real_time                                                5405 ms         5405 ms
BM_Sequential<MMapReader, FNMatch>/mlines:1/real_time                                                    4656 ms         4656 ms
BM_Sequential<MMapReader, FNMatch>/mlines:2/real_time                                                    4727 ms         4727 ms
BM_Sequential<MMapReader, FNMatch>/mlines:4/real_time                                                    4703 ms         4703 ms
BM_Sequential<MMapReader, FNMatch>/mlines:20/real_time                                                   4643 ms         4643 ms
BM_Sequential<MMapReader, FNMatch>/mlines:40/real_time                                                   4623 ms         4623 ms

===== Multi-threaded solution as single producer and many consumers with conditional variables

BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            4901 ms         8557 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           3491 ms         5827 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            4171 ms        11339 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           2540 ms         6726 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           2651 ms         7659 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           2582 ms         7519 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           2974 ms         8123 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           3039 ms         8290 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          3052 ms         8369 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           3058 ms         7719 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           3107 ms         9081 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time          3137 ms         9199 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time          5159 ms         8548 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time         3346 ms         5106 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time          3329 ms         8927 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time         1722 ms         4917 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time         1833 ms         6309 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time         1753 ms         6153 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time         1967 ms         7140 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time         1994 ms         7034 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time        1943 ms         6869 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time         2085 ms         6920 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time         2084 ms         7883 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time        2094 ms         8218 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time             4927 ms         7687 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time            3403 ms         4138 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time             3597 ms         8861 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time            1846 ms         4166 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time            1417 ms         4782 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time            1340 ms         4360 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time            1206 ms         3841 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time            1248 ms         4015 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time           1252 ms         4009 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time             959 ms         6439 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time             949 ms         6369 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time            949 ms         6365 ms

===== Multi-threaded solution as single producer and many consumers without locks

BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time           3875 ms         7748 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time          3462 ms         6921 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time           2563 ms         7689 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time          2200 ms         6597 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time          2256 ms         9021 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time          2873 ms        11485 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time          3199 ms        12730 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time          3171 ms        12575 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time         3274 ms        12901 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time          5107 ms        40146 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time          5181 ms        40571 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time         5260 ms        40781 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time         3834 ms         7668 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time        3235 ms         6470 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time         2083 ms         6250 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time        1649 ms         4943 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time        1515 ms         6053 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time        1920 ms         7673 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time        2068 ms         8207 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time        2045 ms         8071 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time       2044 ms         7982 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time        3590 ms        28000 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        3536 ms        27248 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time       3587 ms        27275 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            3565 ms         6994 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           3251 ms         6376 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            1958 ms         5634 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           1688 ms         4823 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           1187 ms         4385 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           1172 ms         4329 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           1162 ms         4289 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           1242 ms         4600 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          1295 ms         4796 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           1131 ms         8101 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           1130 ms         8090 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time          1129 ms         8088 ms

===== Multi-threaded solution as single producer and many consumers with semaphores (since C++20)

BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time                5423 ms        10833 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time               3652 ms         6847 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time                4156 ms        11871 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time               2670 ms         7302 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time               2758 ms         9092 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time               2830 ms         9180 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time               3150 ms         8446 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time               2966 ms         8395 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time              3221 ms         8809 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time               3608 ms        12391 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time               3380 ms         9971 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time              3400 ms        10028 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time              5468 ms        10909 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time             3534 ms         6152 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time              3306 ms         9710 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time             1755 ms         5201 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time             1850 ms         6904 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time             1843 ms         6786 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time             1908 ms         6819 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time             2107 ms         7626 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time            2062 ms         7550 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time             2824 ms        12277 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time             2411 ms         9437 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time            2394 ms         9459 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time                 4974 ms         9804 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time                3625 ms         5175 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time                 2677 ms         7787 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time                1871 ms         4704 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time                1419 ms         5058 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time                1346 ms         4728 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time                1308 ms         4385 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time                1326 ms         4435 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time               1292 ms         4304 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time                 994 ms         6891 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time                1135 ms         7569 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time               1126 ms         7804 ms
```

- BM_Sequential   - Single-threaded implementation
- BM_MTCondVar    - Multi-threaded implementation as a Producer-Consumer solution using
                    mutex and condition variables.
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

BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time        949 ms          6365 ms

=== Cold buffer cache (after `sync; echo 1 > /proc/sys/vm/drop_caches`):

BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time       4140 ms          6843 ms
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
just burn CPU cores what is not good at all.
So be careful with such lock-free solutions. You should understand what you are doing.
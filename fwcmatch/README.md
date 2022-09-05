
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
C++ standard regex library (regexwildcard.cpp/h) but it works really slow and
I decided not to use this implementation.
Anyway I had no purpose to benchmark wildcard algorithms.
I implemented single-threaded and multi-threaded solutions.
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

But CMakeLists.txt for cmake build system is also provided:
```
cmake -S . -B build
cmake --build build
BENCH_FILENAME="/files/tmp/unison.log" BENCH_PATTERN="*failed*" ./build/fwcmatch-bench
```

Build and runtime dependencies:
- [Google Benchmark](https://github.com/google/benchmark)
  (dev-cpp/benchmark in Gentoo, version 1.6.1 was used)

## The results
<details open>
<summary>

#### Testing environment

</summary>

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
</details>

#### Output
I created a 4GB file from my own unison.log and used the
wildcard pattern \*failed\* for all benchmarks.

Some fields were cut off for shortance.

```
----------------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                                   Time             CPU
----------------------------------------------------------------------------------------------------------------------------------
```
<details open>
<summary>Single-threaded solution</summary>

```
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:1/real_time                                           8995 ms         8993 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:4/real_time                                           9287 ms         9287 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:16/real_time                                          9328 ms         9327 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:32/real_time                                          9332 ms         9331 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:1/real_time                                         8581 ms         8580 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:4/real_time                                         8814 ms         8814 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:16/real_time                                        8784 ms         8784 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:32/real_time                                        8735 ms         8734 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:1/real_time                                            7105 ms         7105 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:4/real_time                                            7265 ms         7264 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:16/real_time                                           7327 ms         7326 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:32/real_time                                           7242 ms         7240 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:1/real_time                                                  11165 ms        11165 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:4/real_time                                                  11446 ms        11446 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:16/real_time                                                 11443 ms        11443 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:32/real_time                                                 11478 ms        11471 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:1/real_time                                                10840 ms        10839 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:4/real_time                                                10984 ms        10984 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:16/real_time                                               10840 ms        10840 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:32/real_time                                               10835 ms        10835 ms
BM_Sequential<MMapReader, FNMatch>/mlines:1/real_time                                                    9292 ms         9292 ms
BM_Sequential<MMapReader, FNMatch>/mlines:4/real_time                                                    9415 ms         9415 ms
BM_Sequential<MMapReader, FNMatch>/mlines:16/real_time                                                   9355 ms         9355 ms
BM_Sequential<MMapReader, FNMatch>/mlines:32/real_time                                                   9361 ms         9361 ms
```
</details>

<details open>
<summary>Multi-threaded solution with conditional variables</summary>

```
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time            8144 ms        14062 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time            6862 ms        11049 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time            6898 ms        10978 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time           6843 ms        10894 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time            4521 ms        14515 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time            4376 ms        14238 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time            4451 ms        14230 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time           4359 ms        14056 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time           4369 ms        14256 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time          4379 ms        14218 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time           4115 ms        13214 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time           3999 ms        12989 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time          3962 ms        13001 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time           4289 ms        12733 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time          4335 ms        12893 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time          7861 ms        12901 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time          6524 ms         9652 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time          6690 ms         9720 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time         6581 ms         9465 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time          3228 ms        11493 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time          3148 ms        11288 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time          3205 ms        11434 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time         3272 ms        11817 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time         3239 ms        11648 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time        3194 ms        11602 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time         2996 ms        10774 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time         3114 ms        11284 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time        3220 ms        11395 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time         3423 ms        12721 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time        3434 ms        12705 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time             8329 ms        12389 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time             6805 ms         8281 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time             6908 ms         8147 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time            6929 ms         7992 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time             2636 ms         8735 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time             2566 ms         8329 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time             2504 ms         8017 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time            2500 ms         8028 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time            2553 ms         8056 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time           2514 ms         7842 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time            2520 ms         8094 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time            2448 ms         7770 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time           2526 ms         7977 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time            1895 ms        12724 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time           1897 ms        12701 ms
```
</details>

<details open>
<summary>Multi-threaded solution with conditional variables (2)</summary>

```
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time          12321 ms        20406 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time           7072 ms        11718 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time           6513 ms        10057 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time          6636 ms        10247 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time           6834 ms        14078 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time           4728 ms        13573 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time           3862 ms        13270 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time          3864 ms        13164 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time          3762 ms        12908 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time         3732 ms        13172 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time          4431 ms        12519 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time          3540 ms        11499 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time         3484 ms        12032 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time          3985 ms        12499 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time         4045 ms        12706 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time        11600 ms        18993 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time         6934 ms        10794 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time         6519 ms         9302 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time        6721 ms         9551 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time         6534 ms        13562 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time         4292 ms        12658 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time         3613 ms        12296 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time        3312 ms        11575 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time        3362 ms        11824 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time       3196 ms        11458 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time        3817 ms        11160 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time        3101 ms        10690 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time       3125 ms        11263 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time        3425 ms        12709 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time       3438 ms        12753 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time            8369 ms        12405 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time            6852 ms         8357 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time            6970 ms         8242 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time           6862 ms         7966 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time            2627 ms         8704 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time            2591 ms         8408 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time            2524 ms         8101 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time           2507 ms         8041 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time           2655 ms         8246 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time          2548 ms         8013 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time           2511 ms         8129 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time           2439 ms         7794 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time          2506 ms         8096 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time           1902 ms        12831 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time          2015 ms        13559 ms
```
</details>

<details open>
<summary>Multi-threaded solution with atomic and busy-waiting</summary>

```
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time           6936 ms        13871 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time           6529 ms        13056 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time           6543 ms        13085 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time          6471 ms        12941 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time           3726 ms        14903 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time           3751 ms        15002 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time           4325 ms        17297 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time          4320 ms        17277 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time          4220 ms        16877 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time         4314 ms        17248 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time          4327 ms        17307 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time          4252 ms        17007 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time         4266 ms        17061 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time          6548 ms        51614 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time         6476 ms        50941 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time         6821 ms        13640 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time         6237 ms        12473 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time         6199 ms        12397 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time        6198 ms        12394 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time         2642 ms        10568 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time         2519 ms        10077 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time         2637 ms        10546 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time        2852 ms        11406 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time        2862 ms        11447 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time       2903 ms        11609 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time        2848 ms        11392 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time        2826 ms        11303 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time       2840 ms        11353 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time        4698 ms        36592 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time       4726 ms        37382 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time            6745 ms        13225 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time            6461 ms        12687 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time            6449 ms        12662 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time           6439 ms        12641 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time            2328 ms         8612 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time            2314 ms         8562 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time            2350 ms         8707 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time           2323 ms         8594 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time           2342 ms         8674 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time          2501 ms         9292 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time           2538 ms         9440 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time           2547 ms         9476 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time          2557 ms         9515 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time           2224 ms        15984 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time          2215 ms        15961 ms
```
</details>

<details open>
<summary>Multi-threaded solution with atomic and busy-waiting (2)</summary>

```
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time               7065 ms        14128 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time               6585 ms        13168 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time               6566 ms        13131 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time              6546 ms        13091 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time               3850 ms        15398 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time               3835 ms        15341 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time               3847 ms        15389 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time              3841 ms        15363 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time              3862 ms        15449 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time             3890 ms        15560 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time              4123 ms        16492 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time              4171 ms        16685 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time             4256 ms        17021 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time              6535 ms        51291 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time             5946 ms        47330 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time             6663 ms        13324 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time             6241 ms        12482 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time             6217 ms        12435 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time            6194 ms        12387 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time             2562 ms        10247 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time             2546 ms        10183 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time             2536 ms        10142 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time            2608 ms        10433 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time            2854 ms        11416 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time           2854 ms        11413 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time            2804 ms        11214 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time            2806 ms        11222 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time           2807 ms        11228 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time            4476 ms        35127 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time           4357 ms        34493 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time                6849 ms        13469 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time                6447 ms        12674 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time                6449 ms        12677 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time               6442 ms        12663 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time                2322 ms         8626 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time                2327 ms         8653 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time                2319 ms         8618 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time               2316 ms         8601 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time               2320 ms         8620 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time              2352 ms         8744 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time               2463 ms         9176 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time               2517 ms         9388 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time              2543 ms         9494 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time               2173 ms        15722 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time              2195 ms        15828 ms
```
</details>

<details open>
<summary>Multi-threaded solution with semaphores (since C++20)</summary>

```
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time                8737 ms        17373 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time                7282 ms        13079 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time                7209 ms        12994 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time               7158 ms        12898 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time                4564 ms        15740 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time                4436 ms        15669 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time                4433 ms        15911 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time               4389 ms        15684 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time               4750 ms        16564 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time              4768 ms        16833 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time               4176 ms        14500 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time               4369 ms        14377 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time              4293 ms        14485 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time               5462 ms        21048 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time              5478 ms        21083 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time              8585 ms        16995 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time              6935 ms        11796 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time              7002 ms        11896 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time             7084 ms        11999 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time              3124 ms        11830 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time              3138 ms        11830 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time              3199 ms        11697 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time             3180 ms        11882 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time             3480 ms        13010 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time            3479 ms        10406 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time             3116 ms        11504 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time             3202 ms        11854 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time            3139 ms        11485 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time             4278 ms        19779 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time            4328 ms        20006 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time                 8794 ms        17079 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time                 7178 ms        10367 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time                 7190 ms        10348 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time                7168 ms        10343 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time                 2546 ms         8999 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time                 2531 ms         8868 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time                 2542 ms         8883 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time                2508 ms         8782 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time                2493 ms         8703 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time               2491 ms         8681 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time                2427 ms         8129 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time                2648 ms         8896 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time               2671 ms         9032 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time                2218 ms        15257 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time               2225 ms        15445 ms
```
</details>

#### Output description
- BM_Sequential   - Single-threaded implementation
- BM_MTCondVar    - Multi-threaded implementation as a Producer-Consumer solution using
                    mutex and condition variables.
- BM_MTCondVar2   - An alternative version of BM_MTCondVar. In theory it has better
                    memory locality but uses more mutex locks or copying of blocks
                    if blocks without a buffer (mmap).
- BM_MTLockFree   - Multi-threaded implementation as a Producer-Consumer solution
                    using atomic and busy-waiting ring buffer based on [this](https://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular).
- BM_MTMPMC       - Similar to BM_MTLockFree but uses MPMCQueue from [here](https://github.com/rigtorp/MPMCQueue).
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
- There is no reason to use number of read/filtered lines more than
  one at one time for single-threaded implementation.
  But it is important for multi-threaded implementation.
- Increasing number of threads in multi-threaded implementation is not always
  optimal decision. But it is because file I/O is bottleneck for this task
  and it can increase thread contention.
- Lock-free/busy-waiting solution is not always the best choice.
- MTSem (semaphores) is similar to the MTCondVar (condition variables)
  in terms of performance but the MTCondVar is a little bit better.
- Memory/data locality is important for general performance.
- For perfomance reason it is better to hold size of memory for
  queue (ring buffer) and all blocks with data less than CPU L3 cache size.

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

## About busy-waiting solutions with atomic
In this solution I used implementation of
ring buffer from [here](https://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular) in the first variant.
This container is thread-safe but only for single producer and single consumer variant.
So I implemented this as a group of pairs "single producer -> single consumer" where
my single producer emulates many producers in a loop. In some cases this solution showed better
performance than solution with conditional variables but not always.
The second variant uses MPMCQueue from [here](https://github.com/rigtorp/MPMCQueue) and has very similar
performance with the first variant.
The main problem with both variants is busy-waiting on CPU when these queues are full or empty.
Look at the column "CPU" to see the proof. And if you set number of threads
more than necessery for optimal processing then a lot of time this code will
just "burn" CPU cores what is not good at all. But such a way can help in some cases and
with law latency for example.
So be careful with such busy-waiting solutions. You should understand what you are doing.

UPD. With MPMCQueue (BM_MTMPMC) I got a really huge regression on 16 threads:

```
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time        4430 ms        35284 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:32/threads:16/mlines:96/process_time/real_time      352768 ms      2801674 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:128/threads:16/mlines:96/process_time/real_time      99997 ms       793749 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:16/mlines:256/process_time/real_time      393153 ms      3126706 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:16/mlines:256/process_time/real_time     228647 ms      1812834 ms
```

BM_MTLockFree also showed regression but not so huge:

```
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time        4235 ms        33703 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:32/threads:16/mlines:96/process_time/real_time        9522 ms        75648 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:128/threads:16/mlines:96/process_time/real_time      11146 ms        87174 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:16/mlines:256/process_time/real_time       11697 ms        89964 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:16/mlines:256/process_time/real_time       9169 ms        72299 ms
```

## About spinlocks
I used spinlocks a lot in the past (as the spin_mutex from Intel TBB)
and you can find a lot of variants on the internet how to implement such a thing
(from std::atomic_flag for example) but I'm not sure that they all work properly.
Also I can recommend to read this: https://www.realworldtech.com/forum/?threadid=189711&curpostid=189723
Nevertheless as I know modern implementations of std::mutex already use similar
thing inside for a short waitings and I think this is enough already.

## Memory locality
This can improve performance but you must be accurate in
a way how to achieve it. I improved memory locality for any reading/filtering
in the case of using a buffer (FGetsReader, FStreamReader). Also I tried to do it
for a ring buffer and made MTCondVar2 but I had to use more mutex locks or
copying of current block and benchmarks showed that for current task the MTCondVar
is better in terms of performance.
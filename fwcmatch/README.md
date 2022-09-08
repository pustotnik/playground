
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
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:1/real_time                                           9241 ms         9241 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:4/real_time                                           9425 ms         9425 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:16/real_time                                          9380 ms         9380 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:32/real_time                                          9283 ms         9282 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:1/real_time                                         8682 ms         8682 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:4/real_time                                         8779 ms         8779 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:16/real_time                                        8690 ms         8690 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:32/real_time                                        8602 ms         8602 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:1/real_time                                            7226 ms         7226 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:4/real_time                                            7281 ms         7281 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:16/real_time                                           7253 ms         7253 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:32/real_time                                           7149 ms         7148 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:1/real_time                                                  11370 ms        11366 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:4/real_time                                                  11478 ms        11478 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:16/real_time                                                 11343 ms        11343 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:32/real_time                                                 11291 ms        11290 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:1/real_time                                                10793 ms        10791 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:4/real_time                                                10872 ms        10872 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:16/real_time                                               10674 ms        10674 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:32/real_time                                               10616 ms        10616 ms
BM_Sequential<MMapReader, FNMatch>/mlines:1/real_time                                                    9262 ms         9261 ms
BM_Sequential<MMapReader, FNMatch>/mlines:4/real_time                                                    9266 ms         9264 ms
BM_Sequential<MMapReader, FNMatch>/mlines:16/real_time                                                   9172 ms         9170 ms
BM_Sequential<MMapReader, FNMatch>/mlines:32/real_time                                                   9126 ms         9123 ms
```
</details>

<details open>
<summary>Multi-threaded solution with conditional variables</summary>

```
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time            7667 ms        13180 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time            6462 ms        10257 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time            6475 ms        10137 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time           6469 ms        10122 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time            3922 ms        13242 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time            3994 ms        13401 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time            4054 ms        13230 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time           3852 ms        12907 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time           3779 ms        12916 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time          3971 ms        13016 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time           3519 ms        11759 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time           3487 ms        12050 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time          3498 ms        12072 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time           3883 ms        12159 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time          3886 ms        12186 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time          7970 ms        13098 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time          6560 ms         9662 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time          6548 ms         9425 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time         6577 ms         9446 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time          3220 ms        11453 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time          3213 ms        11325 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time          3219 ms        11470 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time         3315 ms        11798 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time         3321 ms        11685 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time        3192 ms        11440 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time         2998 ms        10928 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time         3175 ms        11389 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time        3125 ms        11322 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time         3389 ms        12770 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time        3387 ms        12702 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time             8407 ms        12467 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time             6804 ms         8280 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time             6881 ms         8120 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time            6962 ms         8034 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time             2607 ms         8626 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time             2562 ms         8345 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time             2489 ms         8060 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time            2470 ms         7947 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time            2556 ms         7986 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time           2508 ms         7947 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time            2481 ms         8015 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time            2393 ms         7607 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time           2486 ms         7906 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time            1888 ms        12728 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time           1899 ms        12681 ms
```
</details>

<details open>
<summary>Multi-threaded solution with conditional variables (2)</summary>

```
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time          12544 ms        20949 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time           7208 ms        12096 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time           6786 ms        10769 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time          6899 ms        10926 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time           6980 ms        14336 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time           4922 ms        14282 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time           4408 ms        14601 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time          4300 ms        14246 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time          4307 ms        14228 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time         4329 ms        14371 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time          4528 ms        12944 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time          3926 ms        12697 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time         3919 ms        12870 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time          4224 ms        12731 ms
BM_MTCondVar2<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time         4376 ms        13330 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time        11929 ms        19541 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time         6914 ms        10777 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time         6461 ms         9194 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time        6667 ms         9426 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time         6360 ms        13193 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time         4164 ms        12335 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time         3519 ms        11912 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time        3345 ms        11880 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time        3340 ms        11887 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time       3191 ms        11652 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time        3719 ms        10855 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time        3308 ms        11286 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time       2933 ms        10357 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time        3420 ms        12893 ms
BM_MTCondVar2<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time       3393 ms        12785 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time            8547 ms        12671 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time            6998 ms         8512 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time            7071 ms         8368 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time           6946 ms         8064 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time            2799 ms         9416 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time            2689 ms         8837 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time            2643 ms         8742 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time           2629 ms         8562 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time           2683 ms         8825 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time          2695 ms         8647 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time           2641 ms         8701 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time           2552 ms         8247 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time          2643 ms         8489 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time           1934 ms        13353 ms
BM_MTCondVar2<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time          2137 ms        14585 ms
```
</details>

<details open>
<summary>Multi-threaded solution with atomic and busy-waiting</summary>

```
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time           7111 ms        14222 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time           6560 ms        13119 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time           6538 ms        13076 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time          6528 ms        13055 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time           3801 ms        15203 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time           3786 ms        15144 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time           4302 ms        17207 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time          4246 ms        16984 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time          4297 ms        17188 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time         4399 ms        17590 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time          4287 ms        17148 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time          4288 ms        17151 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time         4383 ms        17531 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time          6429 ms        50823 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time         6430 ms        50984 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time         6785 ms        13569 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time         6223 ms        12445 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time         6186 ms        12371 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time        6148 ms        12295 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time         2506 ms        10022 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time         2476 ms         9903 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time         2478 ms         9911 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time        2817 ms        11266 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time        2815 ms        11259 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time       2880 ms        11515 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time        2807 ms        11226 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time        2911 ms        11643 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time       2847 ms        11384 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time        4487 ms        35292 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time       4565 ms        36308 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time            6895 ms        13559 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time            6546 ms        12875 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time            6488 ms        12759 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time           6479 ms        12742 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time            2416 ms         9015 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time            2412 ms         8997 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time            2417 ms         9020 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time           2463 ms         9199 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time           2423 ms         9040 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time          2529 ms         9443 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time           2640 ms         9884 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time           2692 ms        10099 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time          2656 ms         9957 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time           2202 ms        15966 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time          2325 ms        16423 ms
```
</details>

<details open>
<summary>Multi-threaded solution with atomic and busy-waiting (2)</summary>

```
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time               7117 ms        14233 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time               6569 ms        13138 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time               6563 ms        13126 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time              6572 ms        13142 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time               3792 ms        15169 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time               3834 ms        15336 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time               3879 ms        15515 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time              4071 ms        16284 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time              4169 ms        16677 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time             4239 ms        16956 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time              4124 ms        16495 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time              4119 ms        16474 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time             4144 ms        16574 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time              5618 ms        44752 ms
BM_MTMPMC<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time             5599 ms        44631 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time             6768 ms        13533 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time             6249 ms        12497 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time             6223 ms        12444 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time            6209 ms        12415 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time             2521 ms        10083 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time             2516 ms        10065 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time             2518 ms        10070 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time            2578 ms        10313 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time            2820 ms        11278 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time           2845 ms        11380 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time            2772 ms        11087 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time            2889 ms        11556 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time           2799 ms        11196 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time            4116 ms        32752 ms
BM_MTMPMC<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time           4112 ms        32756 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time                7034 ms        13841 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time                6478 ms        12739 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time                6450 ms        12684 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time               6454 ms        12691 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time                2334 ms         8689 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time                2332 ms         8674 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time                2329 ms         8665 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time               2326 ms         8653 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time               2341 ms         8713 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time              2360 ms         8775 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time               2545 ms         9512 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time               2523 ms         9421 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time              2528 ms         9445 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time               2173 ms        15744 ms
BM_MTMPMC<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time              2353 ms        16568 ms
```
</details>

<details open>
<summary>Multi-threaded solution with semaphores (since C++20)</summary>

```
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time                8711 ms        17319 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time                6968 ms        12428 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time                6890 ms        12308 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time               6869 ms        12210 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time                4057 ms        14588 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time                3984 ms        14089 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time                3985 ms        14217 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time               3908 ms        14268 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time               4302 ms        15415 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time              4262 ms        15292 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time               3798 ms        13258 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time               3829 ms        12909 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time              3830 ms        13237 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time               5127 ms        20571 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time              5139 ms        20502 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time              8873 ms        17567 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time              6994 ms        11851 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time              6946 ms        11788 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time             6999 ms        11853 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time              3171 ms        12105 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time              3144 ms        11883 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time              3185 ms        11743 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time             3259 ms        11842 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time             3368 ms        12118 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time            3552 ms        13376 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time             3167 ms        11749 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time             3224 ms        11866 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time            3121 ms        11650 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time             4243 ms        19933 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time            4234 ms        19694 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:16/process_time/real_time                 9042 ms        17578 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:96/process_time/real_time                 7260 ms        10423 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:2/mlines:96/process_time/real_time                 7178 ms        10322 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:32/threads:2/mlines:96/process_time/real_time                7164 ms        10324 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:4/mlines:96/process_time/real_time                 2566 ms         9121 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:96/process_time/real_time                 2500 ms         8767 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:96/process_time/real_time                 2473 ms         8681 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:96/process_time/real_time                2518 ms         8876 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:32/threads:4/mlines:96/process_time/real_time                2516 ms         8837 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:128/threads:4/mlines:96/process_time/real_time               2553 ms         9021 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:256/process_time/real_time                2506 ms         8510 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:256/process_time/real_time                2625 ms         8874 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:256/process_time/real_time               2733 ms         9312 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:256/process_time/real_time                2206 ms        15385 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:256/process_time/real_time               2214 ms        15461 ms
```
</details>

<details open>
<summary>Multi-threaded solution with file reading lock (OpenMP implementation)</summary>

```
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:2/mlines:96/process_time/real_time                 4551 ms         9100 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:2/mlines:256/process_time/real_time                4409 ms         8817 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:2/mlines:512/process_time/real_time                4355 ms         8708 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:4/mlines:96/process_time/real_time                 2852 ms        11403 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:4/mlines:256/process_time/real_time                2630 ms        10525 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:4/mlines:512/process_time/real_time                2871 ms        11487 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:8/mlines:96/process_time/real_time                 3726 ms        29677 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:8/mlines:256/process_time/real_time                3550 ms        28068 ms
BM_MTLockRead<FStreamReader, MyWildcardMatch>/threads:8/mlines:512/process_time/real_time                3473 ms        27643 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:2/mlines:96/process_time/real_time                    3731 ms         7363 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:2/mlines:256/process_time/real_time                   3709 ms         7215 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:2/mlines:512/process_time/real_time                   3680 ms         7159 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:4/mlines:96/process_time/real_time                    1995 ms         7380 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:4/mlines:256/process_time/real_time                   1979 ms         7311 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:4/mlines:512/process_time/real_time                   2008 ms         7420 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:8/mlines:96/process_time/real_time                    2168 ms        15713 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:8/mlines:256/process_time/real_time                   2145 ms        15627 ms
BM_MTLockRead<MMapReader, MyWildcardMatch>/threads:8/mlines:512/process_time/real_time                   2137 ms        15561 ms
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
- BM_MTLockRead   - Multi-threaded implementation with locking of whole file reading
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
- FNMatch works slower than MyWildcardMatch just because fnmatch
  requires C string terminated with '\0' and I had to make copy of each file
  line because original text lines from afile don't have this symbol while
  MyWildcardMatch can be used just with pointers to begin/end of string without
  copying of text to create new string with '\0'.
- There is no reason to use number of read/filtered lines more than
  one at one time for single-threaded implementation.
  But it is important for multi-threaded implementation.
- Increasing number of threads in multi-threaded implementation is not always
  optimal decision. The main problems are thread contention and the fact
  that file I/O is bottleneck for this task.
- Lock-free/busy-waiting solution is not always the best choice.
- MTSem (semaphores) is similar to the MTCondVar (condition variables)
  in terms of performance but the MTCondVar is a little bit better.
- Memory/data locality can be important for general performance.
- For performance reason it is better to hold size of memory for
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
For MTLockFree I used implementation of
ring buffer from [here](https://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular)
with some extentions.
This container is thread-safe but only for single producer and single consumer variant.
So I implemented this as a group of pairs "single producer -> single consumer" where
my single producer emulates many producers in a loop. In some cases this solution showed better
performance than solution with conditional variables (MTCondVar) but not always.
The second variant uses MPMCQueue from [here](https://github.com/rigtorp/MPMCQueue) and has very similar
performance with the MTLockFree.
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

## About MTLockRead
It is simplest way to implement a solution for the problem. We read and filter
in each thread but for reading we use mutex lock because we cannot read a single file in different
threads concurrently. But there is no any queue in this case.
I implemented this in two ways: with std::thread/std::mutex and with OpenMP.
The OpenMP variant is shorter in code and runs a little bit faster (I don't know why).
It is interesting that this solution shows a really good performance.

## About spinlocks
I used spinlocks a lot in the past (as the spin_mutex from Intel TBB)
and you can find a lot of variants on the internet how to implement such a thing
(from std::atomic_flag for example) but I'm not sure that they all work properly.
Also I can recommend to read [this](https://www.realworldtech.com/forum/?threadid=189711&curpostid=189723).
Nevertheless as I know modern implementations of std::mutex already use similar
thing inside for short waitings and I think this can be enough in many cases.
But it depends on implementation and hardware.

## Memory locality
This can improve performance but you must be accurate in
a way how to achieve it. I improved memory locality for any reading/filtering
in the case of using a buffer (FGetsReader, FStreamReader). Also I tried to do it
for a ring buffer and made MTCondVar2 but I had to use more mutex locks or
copying of current block and my benchmarks showed that the MTCondVar
is better in terms of performance.

## What I didn't try
Of course there are several other ways to implement solution for this problem
and I mean use of some tasking system in explicit or implicit way. At least I know
about Intel TBB/OneTBB (flow graph or direct use of tasks), OpenMP (task directive) and
Apple’s [libdispatch](https://github.com/apple/swift-corelibs-libdispatch)
a.k.a. Grand Central Dispatch. In theory the use of such a system can produce the
fastest and more scalable solution and maybe later I will try one of them.
Any way the effective implementation of such a system is not trivial task and it
is better to use something ready to use.
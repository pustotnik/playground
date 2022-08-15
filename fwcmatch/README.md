
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
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:1/real_time                                           4548 ms         4547 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:2/real_time                                           4617 ms         4617 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:4/real_time                                           4655 ms         4655 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:20/real_time                                          4642 ms         4642 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:40/real_time                                          4616 ms         4616 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:1/real_time                                         4343 ms         4343 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:2/real_time                                         4407 ms         4406 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:4/real_time                                         4470 ms         4469 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:20/real_time                                        4412 ms         4411 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:40/real_time                                        4388 ms         4387 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:1/real_time                                            3590 ms         3589 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:2/real_time                                            3631 ms         3630 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:4/real_time                                            3653 ms         3652 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:20/real_time                                           3634 ms         3633 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:40/real_time                                           3605 ms         3605 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:1/real_time                                                   5627 ms         5627 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:2/real_time                                                   5742 ms         5741 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:4/real_time                                                   5801 ms         5799 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:20/real_time                                                  5747 ms         5745 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:40/real_time                                                  5766 ms         5764 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:1/real_time                                                 5501 ms         5499 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:2/real_time                                                 5577 ms         5576 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:4/real_time                                                 5628 ms         5627 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:20/real_time                                                5508 ms         5506 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:40/real_time                                                5501 ms         5499 ms
BM_Sequential<MMapReader, FNMatch>/mlines:1/real_time                                                    4716 ms         4716 ms
BM_Sequential<MMapReader, FNMatch>/mlines:2/real_time                                                    4782 ms         4782 ms
BM_Sequential<MMapReader, FNMatch>/mlines:4/real_time                                                    4749 ms         4749 ms
BM_Sequential<MMapReader, FNMatch>/mlines:20/real_time                                                   4687 ms         4687 ms
BM_Sequential<MMapReader, FNMatch>/mlines:40/real_time                                                   4665 ms         4665 ms

BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time           4900 ms        13179 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time          2354 ms         6364 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time          2499 ms         8003 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time          2610 ms         8166 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time          2740 ms         7396 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time          2808 ms         7509 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time         2766 ms         7429 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time          3024 ms        10510 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time          3120 ms         9384 ms
BM_MTProdCons<FGetsReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time         3085 ms         9332 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time         5383 ms        14013 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time        1794 ms         5145 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time        1852 ms         6636 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time        1916 ms         6758 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time        1844 ms         6271 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time        1828 ms         6214 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time       1936 ms         6509 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time        2497 ms         9809 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        2194 ms         8006 ms
BM_MTProdCons<FStreamReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time       2303 ms         8435 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            6660 ms        16527 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           1960 ms         4644 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           1445 ms         4849 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           1549 ms         5167 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           1367 ms         4370 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           1315 ms         4232 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          1326 ms         4249 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           1168 ms         7821 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           1120 ms         7500 ms
BM_MTProdCons<MMapReader, MyWildcardMatch>/qsize:16/threads:8/mlines:500/process_time/real_time          1113 ms         7448 ms
```

- BM_Sequential - single-threaded implementation
- BM_MTProdCons - multi-threaded implementation as almost classical Producer-Consumer problem using
                  mutex and condition variable. It is some variant of single producer and multiple consumers.
- FGetsReader - the fgets is used
- FStreamReader - the iostream is used
- MMapReader - the mmap is ised
- MyWildcardMatch - manual implementation of wildcard matching algorithm
- FNMatch - the fnmatch is used
- mlines - max number of file lines read/filtered lines at one time
- qsize - size of queue for multi-threaded implementation
- threads - number of threads for multi-threaded implementation
- column 'CPU' means sum of time from all used CPUs.

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
- It is optimal to set size of queue to number of threads.
- Increasing number of threads in multi-threaded implementation is not always
  optimal decision. But it is because file I/O is bottleneck for this task.

## About mmap
The **mmap** in these benchmarks are fastest but you should to know that it was warm run and the
input file could fit in the memory. For example if you clear pagecache with this command:
```
sync; echo 1 > /proc/sys/vm/drop_caches
```
then the first run of any test with mmap can show noticeable regression.
As I understand the mmap is a good method to read file several times and
for a random access (especially on SSD).
Also I can recommend to read [this](https://sasha-f.medium.com/why-mmap-is-faster-than-system-calls-24718e75ab37).
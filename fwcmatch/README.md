
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

BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:1/real_time                                           9002 ms         9001 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:4/real_time                                           9257 ms         9257 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:20/real_time                                          9220 ms         9220 ms
BM_Sequential<FGetsReader, MyWildcardMatch>/mlines:40/real_time                                          9262 ms         9262 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:1/real_time                                         8563 ms         8563 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:4/real_time                                         8857 ms         8857 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:20/real_time                                        8815 ms         8815 ms
BM_Sequential<FStreamReader, MyWildcardMatch>/mlines:40/real_time                                        8757 ms         8756 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:1/real_time                                            7061 ms         7061 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:4/real_time                                            7247 ms         7247 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:20/real_time                                           7271 ms         7271 ms
BM_Sequential<MMapReader, MyWildcardMatch>/mlines:40/real_time                                           7183 ms         7183 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:1/real_time                                                  11071 ms        11070 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:4/real_time                                                  11325 ms        11325 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:20/real_time                                                 11204 ms        11202 ms
BM_Sequential<FGetsReader, FNMatch>/mlines:40/real_time                                                 11270 ms        11270 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:1/real_time                                                10667 ms        10667 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:4/real_time                                                10927 ms        10925 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:20/real_time                                               10839 ms        10838 ms
BM_Sequential<FStreamReader, FNMatch>/mlines:40/real_time                                               10724 ms        10724 ms
BM_Sequential<MMapReader, FNMatch>/mlines:1/real_time                                                    9119 ms         9119 ms
BM_Sequential<MMapReader, FNMatch>/mlines:4/real_time                                                    9253 ms         9253 ms
BM_Sequential<MMapReader, FNMatch>/mlines:20/real_time                                                   9126 ms         9126 ms
BM_Sequential<MMapReader, FNMatch>/mlines:40/real_time                                                   9094 ms         9094 ms

===== Multi-threaded solution as single producer and many consumers with conditional variables

BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            9725 ms        16890 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           6931 ms        11452 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            6910 ms        19269 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           4464 ms        12354 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           4858 ms        14956 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           4705 ms        14966 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           5926 ms        16483 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           5617 ms        15072 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          5609 ms        15092 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           5809 ms        14753 ms
BM_MTCondVar<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           5662 ms        16801 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time          9923 ms        16435 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time         6749 ms        10121 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time          6565 ms        17654 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time         3422 ms         9762 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time         3623 ms        12658 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time         3590 ms        12540 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time         3405 ms        11899 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time         3593 ms        12306 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time        3843 ms        13423 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time         4084 ms        13705 ms
BM_MTCondVar<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time         4161 ms        16146 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time             9480 ms        14734 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time            6840 ms         8286 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time             7045 ms        17452 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time            3655 ms         8322 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time            2843 ms         9666 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time            2607 ms         8577 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time            2402 ms         7725 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time            2438 ms         7837 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time           2439 ms         7878 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time            1897 ms        13059 ms
BM_MTCondVar<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time            2105 ms        13976 ms

===== Multi-threaded solution as single producer and many consumers without locks

BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time           7875 ms        15751 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time          6766 ms        13530 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time           5032 ms        15096 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time          4354 ms        13060 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time          4256 ms        17017 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time          6072 ms        24280 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time          6407 ms        25562 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time          6186 ms        24634 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time         6457 ms        25615 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time         10286 ms        79957 ms
BM_MTLockFree<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time         10269 ms        81054 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time         7779 ms        15557 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time        6422 ms        12843 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time         4350 ms        13047 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time        3307 ms         9919 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time        3031 ms        12123 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time        3819 ms        15273 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time        3974 ms        15886 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time        3900 ms        15566 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time       3912 ms        15519 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time        7437 ms        58185 ms
BM_MTLockFree<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time        7367 ms        57775 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time            7170 ms        14124 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time           6480 ms        12754 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time            3970 ms        11503 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time           3368 ms         9697 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time           2332 ms         8723 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time           2329 ms         8704 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time           2403 ms         8989 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time           2554 ms         9591 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time          2554 ms         9587 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time           2269 ms        16165 ms
BM_MTLockFree<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time           2225 ms        16107 ms

===== Multi-threaded solution as single producer and many consumers with semaphores (since C++20)

BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time               10844 ms        21652 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time               7282 ms        13430 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time                8092 ms        22411 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time               5429 ms        13508 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time               5013 ms        17125 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time               5042 ms        17082 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time               5939 ms        16109 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time               6023 ms        16591 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time              6093 ms        16923 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time               7244 ms        25240 ms
BM_MTSem<FGetsReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time               6677 ms        19933 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time             10588 ms        21127 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time             7098 ms        12284 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time              8377 ms        20039 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time             4284 ms        10724 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time             3596 ms        13451 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time             3727 ms        13451 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time             3667 ms        13458 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time             3882 ms        14253 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time            3941 ms        14455 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time             5569 ms        24509 ms
BM_MTSem<FStreamReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time             4681 ms        18752 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:10/process_time/real_time                 9931 ms        19610 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:2/threads:2/mlines:100/process_time/real_time                7207 ms        10297 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:10/process_time/real_time                 5585 ms        15553 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:3/threads:3/mlines:100/process_time/real_time                3889 ms         9587 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:1/threads:4/mlines:100/process_time/real_time                2710 ms         9753 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:100/process_time/real_time                2598 ms         9229 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:4/threads:4/mlines:500/process_time/real_time                2557 ms         8608 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:4/mlines:500/process_time/real_time                2594 ms         8767 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:16/threads:4/mlines:500/process_time/real_time               2602 ms         8793 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:200/process_time/real_time                2145 ms        15117 ms
BM_MTSem<MMapReader, MyWildcardMatch>/qsize:8/threads:8/mlines:500/process_time/real_time                2208 ms        15476 ms

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

## About spinlocks
I used spinlocks a lot in the past (as the spin_mutex from Intel TBB)
and you can find a lot of variants on the internet how to implement such a thing
(from std::atomic_flag for example) but I'm not sure that they all work properly.
Also I can recommend to read this: https://www.realworldtech.com/forum/?threadid=189711&curpostid=189723
Nevertheless as I know modern implementations of std::mutex already use similar
thing inside for a short waitings and I think this is enough already.
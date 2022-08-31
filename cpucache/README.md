

Inspired by

- https://www.youtube.com/watch?v=WDIkqP4JbkE
- https://www.aristeia.com/TalkNotes/codedive-CPUCachesHandouts.pdf

## Results

- Hardware: HP Elitebook 850 g5
- OS: Gentoo Linux (kernel 5.10.27)
- C++ compiler: g++ 11.2.1 (-std=c++17 -fno-exceptions -fno-rtti -O3)

### traversals:

```
----------------------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------
BM_Traverse/8/8/1             19.1 ns         19.1 ns     36647924 Result=2.08k
BM_Traverse/8/8/0             30.4 ns         30.4 ns     23122816 Result=2.08k
BM_Traverse/28/28/1            121 ns          121 ns      5769790 Result=307.72k
BM_Traverse/28/28/0            352 ns          352 ns      1984828 Result=307.72k
BM_Traverse/128/128/1         2060 ns         2060 ns       338022 Result=134.226M
BM_Traverse/128/128/0         8889 ns         8889 ns        77098 Result=134.226M
BM_Traverse/4096/4096/1    3879576 ns      3879577 ns          180 Result=8.38861M
BM_Traverse/4096/4096/0   98508099 ns     98508047 ns            7 Result=8.38861M
BM_Traverse/8192/8192/1   15593918 ns     15593531 ns           45 Result=33.5544M
BM_Traverse/8192/8192/0  521436622 ns    521436683 ns            1 Result=33.5544M
```

### falsesharing:

```
--------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------------------------------------
BM_Scalability/threads:1/good?:0/dim:4096/real_time   16771520 ns        80667 ns           42 Result=8.38861M
BM_Scalability/threads:2/good?:0/dim:4096/real_time   11434666 ns       101998 ns           61 Result=8.38861M
BM_Scalability/threads:3/good?:0/dim:4096/real_time    8197160 ns       119276 ns           83 Result=8.38861M
BM_Scalability/threads:4/good?:0/dim:4096/real_time    7309021 ns       117570 ns          110 Result=8.38861M
BM_Scalability/threads:5/good?:0/dim:4096/real_time    7168174 ns       148006 ns          108 Result=8.38861M
BM_Scalability/threads:6/good?:0/dim:4096/real_time    5771259 ns       154450 ns          127 Result=8.38861M
BM_Scalability/threads:7/good?:0/dim:4096/real_time    4902542 ns       183340 ns          141 Result=8.38861M
BM_Scalability/threads:8/good?:0/dim:4096/real_time    9033103 ns       208154 ns           76 Result=8.38861M
BM_Scalability/threads:1/good?:1/dim:4096/real_time    5011191 ns        69785 ns          140 Result=8.38861M
BM_Scalability/threads:2/good?:1/dim:4096/real_time    3125413 ns        84185 ns          225 Result=8.38861M
BM_Scalability/threads:3/good?:1/dim:4096/real_time    3125411 ns       107920 ns          226 Result=8.38861M
BM_Scalability/threads:4/good?:1/dim:4096/real_time    3113566 ns       106853 ns          239 Result=8.38861M
BM_Scalability/threads:5/good?:1/dim:4096/real_time    3046312 ns       198810 ns          234 Result=8.38861M
BM_Scalability/threads:6/good?:1/dim:4096/real_time    2637682 ns       220414 ns          267 Result=8.38861M
BM_Scalability/threads:7/good?:1/dim:4096/real_time    2592035 ns       259024 ns          271 Result=8.38861M
BM_Scalability/threads:8/good?:1/dim:4096/real_time    2567778 ns       233933 ns          276 Result=8.38861M
BM_Scalability/threads:1/good?:0/dim:8192/real_time   66881580 ns        84914 ns           10 Result=33.5544M
BM_Scalability/threads:2/good?:0/dim:8192/real_time   43847440 ns       117894 ns           16 Result=33.5544M
BM_Scalability/threads:3/good?:0/dim:8192/real_time   30946188 ns       123918 ns           22 Result=33.5544M
BM_Scalability/threads:4/good?:0/dim:8192/real_time   25323279 ns       144379 ns           29 Result=33.5544M
BM_Scalability/threads:5/good?:0/dim:8192/real_time   22828232 ns       154787 ns           31 Result=33.5544M
BM_Scalability/threads:6/good?:0/dim:8192/real_time   22112299 ns       192347 ns           33 Result=33.5544M
BM_Scalability/threads:7/good?:0/dim:8192/real_time   19707796 ns       220418 ns           36 Result=33.5544M
BM_Scalability/threads:8/good?:0/dim:8192/real_time   35656505 ns       234997 ns           20 Result=33.5544M
BM_Scalability/threads:1/good?:1/dim:8192/real_time   19538621 ns        70233 ns           37 Result=33.5544M
BM_Scalability/threads:2/good?:1/dim:8192/real_time   12937568 ns        89495 ns           56 Result=33.5544M
BM_Scalability/threads:3/good?:1/dim:8192/real_time   13226643 ns       148126 ns           59 Result=33.5544M
BM_Scalability/threads:4/good?:1/dim:8192/real_time   12894450 ns       141165 ns           61 Result=33.5544M
BM_Scalability/threads:5/good?:1/dim:8192/real_time   11470828 ns       193396 ns           65 Result=33.5544M
BM_Scalability/threads:6/good?:1/dim:8192/real_time   10308191 ns       223546 ns           68 Result=33.5544M
BM_Scalability/threads:7/good?:1/dim:8192/real_time   10146075 ns       260097 ns           71 Result=33.5544M
BM_Scalability/threads:8/good?:1/dim:8192/real_time   10153784 ns       259482 ns           72 Result=33.5544M
```

- Hardware: HP Elitebook 850 g5
- OS: Gentoo Linux (kernel 6.1.81)
- C++ compiler: g++ 13.2.1 (-std=c++17 -fno-exceptions -fno-rtti -O3)

### containers:
I wanted to make some micro benchmarks to see a real performance.

#### Searching for some small string key in popular C++ containers vector, map and unordered_map of small size

```
--------------------------------------------------------------------------------------------------------------
Benchmark                                                    Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------------------------------------
BM_SearchInVector/search position:0                       5.70 ns         5.70 ns    126602219 Result=101
BM_SearchInVector/search position:5                       26.7 ns         26.7 ns     27509634 Result=106
BM_SearchInVector/search position:10                      45.2 ns         45.2 ns     15391794 Result=111
BM_SearchInVector/search position:13                      57.8 ns         57.8 ns     12003278 Result=114
BM_SearchInVector/search position:19                      92.3 ns         92.3 ns      8566123 Result=120
BM_SearchInMap/search position:0                          21.8 ns         21.8 ns     33056310 Result=101
BM_SearchInMap/search position:5                          24.6 ns         24.6 ns     27656735 Result=106
BM_SearchInMap/search position:10                         30.8 ns         30.8 ns     22757261 Result=111
BM_SearchInMap/search position:13                         29.1 ns         29.1 ns     24037469 Result=114
BM_SearchInMap/search position:19                         31.0 ns         31.0 ns     23106350 Result=120
BM_SearchInUnorderedMap/search position:0                 31.6 ns         31.6 ns     22149678 Result=101
BM_SearchInUnorderedMap/search position:5                 45.1 ns         45.1 ns     15566287 Result=106
BM_SearchInUnorderedMap/search position:10                73.2 ns         73.2 ns      9420544 Result=111
BM_SearchInUnorderedMap/search position:13                21.7 ns         21.7 ns     32427941 Result=114
BM_SearchInUnorderedMap/search position:19                4.01 ns         4.01 ns    174469524 Result=120
```

I often see assertions that std::vector is often the fastest container for small sizes and I agree with
this for some cases but it is better to check your cases how it is fast in reality. Also people not
always remember that hash table (unordered_map) has complexity O(1) only in average case which does
not mean that it has such complexity in all cases. Provided results for this particular case shows that
map (balanced tree) is the best in average case.

#### Comparison for "not equality" of simple struct with std::string

```
--------------------------------------------------------------------------------------------------------------------
Benchmark                                                          Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------------------------------------------------
BM_NotEqualStandard/str len:10/vec size:1/equal:0               6.02 ns         6.02 ns    112282418 Result=1
BM_NotEqualStandard/str len:20/vec size:1/equal:0               6.31 ns         6.31 ns    115521836 Result=1
BM_NotEqualStandard/str len:50/vec size:1/equal:0               6.96 ns         6.96 ns    103197513 Result=1
BM_NotEqualStandard/str len:10/vec size:5/equal:0               27.4 ns         27.4 ns     25496517 Result=1
BM_NotEqualStandard/str len:10/vec size:5/equal:1               25.8 ns         25.8 ns     27141636 Result=0
BM_NotEqualStandard/str len:20/vec size:5/equal:0               27.4 ns         27.4 ns     25426085 Result=1
BM_NotEqualStandard/str len:20/vec size:5/equal:1               25.8 ns         25.8 ns     27180419 Result=0
BM_NotEqualStandard/str len:50/vec size:5/equal:0               28.8 ns         28.8 ns     24245164 Result=1
BM_NotEqualStandard/str len:50/vec size:5/equal:1               30.4 ns         30.5 ns     23005973 Result=0
BM_NotEqualHash/str len:10/vec size:1/equal:0                   19.5 ns         19.5 ns     36041174 Result=1
BM_NotEqualHash/str len:20/vec size:1/equal:0                   24.3 ns         24.3 ns     28846185 Result=1
BM_NotEqualHash/str len:50/vec size:1/equal:0                   30.7 ns         30.7 ns     22545190 Result=1
BM_NotEqualHash/str len:10/vec size:5/equal:0                   88.4 ns         88.4 ns      7879645 Result=1
BM_NotEqualHash/str len:10/vec size:5/equal:1                   88.5 ns         88.5 ns      7899566 Result=0
BM_NotEqualHash/str len:20/vec size:5/equal:0                    109 ns          109 ns      6331864 Result=1
BM_NotEqualHash/str len:20/vec size:5/equal:1                    109 ns          109 ns      6410806 Result=0
BM_NotEqualHash/str len:50/vec size:5/equal:0                    146 ns          146 ns      4783480 Result=1
BM_NotEqualHash/str len:50/vec size:5/equal:1                    146 ns          146 ns      4784534 Result=0
BM_NotEqualHashPartlyCached/str len:10/vec size:1/equal:0       11.4 ns         11.4 ns     61351172 Result=1
BM_NotEqualHashPartlyCached/str len:20/vec size:1/equal:0       12.8 ns         12.8 ns     54495538 Result=1
BM_NotEqualHashPartlyCached/str len:50/vec size:1/equal:0       17.2 ns         17.2 ns     42236119 Result=1
BM_NotEqualHashPartlyCached/str len:10/vec size:5/equal:0       48.9 ns         48.9 ns     14323495 Result=1
BM_NotEqualHashPartlyCached/str len:10/vec size:5/equal:1       48.9 ns         48.9 ns     14325509 Result=0
BM_NotEqualHashPartlyCached/str len:20/vec size:5/equal:0       57.8 ns         57.8 ns     12081800 Result=1
BM_NotEqualHashPartlyCached/str len:20/vec size:5/equal:1       57.8 ns         57.8 ns     12087364 Result=0
BM_NotEqualHashPartlyCached/str len:50/vec size:5/equal:0       75.9 ns         75.9 ns      9199248 Result=1
BM_NotEqualHashPartlyCached/str len:50/vec size:5/equal:1       75.9 ns         75.9 ns      9199531 Result=0
BM_NotEqualHashFullyCached/str len:10/vec size:1/equal:0        3.68 ns         3.68 ns    190341240 Result=1
BM_NotEqualHashFullyCached/str len:20/vec size:1/equal:0        3.68 ns         3.68 ns    190342571 Result=1
BM_NotEqualHashFullyCached/str len:50/vec size:1/equal:0        3.68 ns         3.68 ns    190098589 Result=1
BM_NotEqualHashFullyCached/str len:10/vec size:5/equal:0        8.90 ns         8.90 ns     78199233 Result=1
BM_NotEqualHashFullyCached/str len:10/vec size:5/equal:1        8.90 ns         8.90 ns     78708960 Result=0
BM_NotEqualHashFullyCached/str len:20/vec size:5/equal:0        8.91 ns         8.91 ns     78559499 Result=1
BM_NotEqualHashFullyCached/str len:20/vec size:5/equal:1        8.89 ns         8.89 ns     73769733 Result=0
BM_NotEqualHashFullyCached/str len:50/vec size:5/equal:0        8.96 ns         8.96 ns     77393508 Result=1
BM_NotEqualHashFullyCached/str len:50/vec size:5/equal:1        8.92 ns         8.92 ns     77594027 Result=0
```
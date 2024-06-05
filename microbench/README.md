
- Hardware: HP Elitebook 850 g5
- OS: Gentoo Linux (kernel 6.1.81)
- C++ compiler: g++ 13.2.1 (-std=c++17 -fno-exceptions -fno-rtti -O3)

### containers:
I wanted to make some benchmarks to see a real performance of searching for some small
string key in popular C++ containers vector, map and unordered_map of small size.

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
not mean that it has such complexity in all cases. Real results for this particular case shows that
map (balanced tree) is the best in average case.
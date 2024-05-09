
- Hardware: HP Elitebook 850 g5
- OS: Gentoo Linux (kernel 6.1.81)
- C++ compiler: g++ 13.2.1 (-std=c++17 -fno-exceptions -fno-rtti -O3)

### containers:
I wanted to make some benchmarks to see a real performance of searching for some small
string key in popular C++ containers vector, map and unordered_map of small size.

```
-----------------------------------------------------------------------------------------------------
Benchmark                                           Time             CPU   Iterations UserCounters...
-----------------------------------------------------------------------------------------------------
BM_SearchInVector/search position:0              5.78 ns         5.78 ns    121070905 Result=101
BM_SearchInVector/search position:10             46.9 ns         46.9 ns     13272782 Result=111
BM_SearchInVector/search position:19             84.9 ns         84.9 ns      7889924 Result=120
BM_SearchInMap/search position:0                 22.4 ns         22.4 ns     31918241 Result=101
BM_SearchInMap/search position:10                30.5 ns         30.5 ns     23206417 Result=111
BM_SearchInMap/search position:19                29.9 ns         29.9 ns     23420345 Result=120
BM_SearchInUnorderedMap/search position:0        38.0 ns         38.0 ns     18310081 Result=101
BM_SearchInUnorderedMap/search position:10       76.2 ns         76.2 ns      7491624 Result=111
BM_SearchInUnorderedMap/search position:19       5.02 ns         5.02 ns    114807964 Result=120
```

I often see assertions that std::vector is often the fastest container for small sizes and I agree with
this for some cases but it is better to check your cases how it is fast in reality. Also people not
always remember that hash table (unordered_map) has complexity O(1) only in average case which does
not mean that it has such complexity in all cases. Real results for this particular case shows that
map (balanced tree) is the best in average case.
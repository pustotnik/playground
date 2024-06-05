
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

#include <benchmark/benchmark.h>

using namespace std;

namespace {

constexpr size_t SMALL_CONTAINER_SIZE = 20;
constexpr const char CHARSET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

using StrItem = pair<std::string, size_t>;

// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
inline void hashCombine(std::size_t& /*seed*/) { }

template <typename T, typename... Rest>
inline void hashCombine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hashCombine(seed, rest...);
}

std::string genRandomStr(size_t len = 18) {
    std::string str(CHARSET, len);
    for(size_t i = 0; i < str.size(); ++i) {
        swap(str[i], str[rand() % str.size()]);
    }
    return str;
}

std::vector<StrItem> genInputForSearchKey(size_t size) {
    std::vector<StrItem> result;
    // we want random strings but we want the same random strings for every benchmark
    srand(11);
    for(size_t i = 0; i < size; ++i) {
        result.push_back(StrItem(genRandomStr(), i + 101));
    }

    return result;
}

const std::vector<StrItem> inputForSearchKey = genInputForSearchKey(SMALL_CONTAINER_SIZE);

class SomeArgs {
public:
    std::string& str() { _hash_ready = false; return _str; }
    size_t& v1() { _hash_ready = false; return _v1; }
    int& v2() { _hash_ready = false; return _v2; }
    bool& b1() { _hash_ready = false; return _b1; }

    size_t hash(bool cached) const noexcept {
        if(cached) {
            if(_hash_ready) {
                return _hashVal;
            }
            _hash_ready = true;
        }
        return _hash();
    }

    bool notEqual(const SomeArgs& other) const noexcept {
        return !(_b1 == other._b1 && _v1 == other._v1 && _v2 == other._v2 && _str == other._str);
    }

    /*
    We cannot use std::hash to compare for equality (==) without implementing of some solution
    to solve collisions. But we can use it to compare for not equality (!=).
    */
    bool notEqualByHash(const SomeArgs& other) const noexcept {
        return hash(false) != other.hash(false);
    }
    bool notEqualByHashCached(const SomeArgs& other) const noexcept {
        return hash(true) != other.hash(true);
    }

private:
    // this order of variables for the best alignment in memory
    std::string _str;
    mutable size_t _hashVal = 0;
    size_t _v1 = 0;
    int _v2 = 0;
    bool _b1 = false;
    mutable bool _hash_ready = false;

    size_t _hash() const noexcept {
        _hashVal = 0;
        hashCombine(_hashVal, _str, _v1, _v2, _b1);
        return _hashVal;
    }
};

std::vector<SomeArgs> genInputForNotEqual(size_t strLen, size_t vecSize) {

    std::vector<SomeArgs> result(vecSize);
    // we want random strings but we want the same random strings for every benchmark
    srand(14);
    for(auto& val: result) {
        val.str() = genRandomStr(strLen);
        val.v1() = rand();
        val.v2() = rand();
        val.b1() = rand() % 2;
    }

    return result;
}

std::vector<SomeArgs> genInputForCmpNotEqual(const std::vector<SomeArgs>& input, bool same) {

    auto other = input;
    if (!same) {
        for(auto& val: other) {
            val.str() = genRandomStr(val.str().size());
        }
    }

    return other;
}

} // namespace

//////////////////////////////////////////////////////////////////////
// SEARCH KEY IN CONTAINERS

static void genArgumentsForSearchKey(benchmark::internal::Benchmark* b) {
    b
    ->Args({0,})
    ->Args({static_cast<long>(inputForSearchKey.size()/4), })
    ->Args({static_cast<long>(inputForSearchKey.size()/2), })
    ->Args({static_cast<long>(inputForSearchKey.size()/1.5), })
    ->Args({static_cast<long>(inputForSearchKey.size()-1), })
    ->ArgNames({"search position", });
}

static void BM_SearchInVector(benchmark::State& state) {
    const size_t pos = state.range(0);

    std::vector<StrItem> container = inputForSearchKey;
    auto findStr = inputForSearchKey[pos].first;

    int found = 0;
    for (auto _ : state) {
        for(size_t i = 0; i < inputForSearchKey.size(); ++i) {
            if(findStr == container[i].first) {
                found = container[i].second;
                break;
            }
        }
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInVector)->Apply(genArgumentsForSearchKey);

static void BM_SearchInMap(benchmark::State& state) {
    const size_t pos = state.range(0);

    std::map<std::string, size_t> container;
    for(auto [key, val]: inputForSearchKey) {
        container[key] = val;
    }

    auto findStr = inputForSearchKey[pos].first;
    int found = 0;
    for (auto _ : state) {
        found = container.find(findStr)->second;
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInMap)->Apply(genArgumentsForSearchKey);

static void BM_SearchInUnorderedMap(benchmark::State& state) {
    const size_t pos = state.range(0);

    std::unordered_map<std::string, size_t> container;
    for(auto [key, val]: inputForSearchKey) {
        container[key] = val;
    }

    auto findStr = inputForSearchKey[pos].first;
    int found = 0;
    for (auto _ : state) {
        found = container.find(findStr)->second;
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInUnorderedMap)->Apply(genArgumentsForSearchKey);

//////////////////////////////////////////////////////////////////////
// NOT EQUAL
static void genArgumentsForNotEqual(benchmark::internal::Benchmark* b) {
    b
    ->Args({10, 1, false})
    ->Args({20, 1, false})
    ->Args({50, 1, false})

    ->Args({10, 5, false})
    ->Args({10, 5, true})
    ->Args({20, 5, false})
    ->Args({20, 5, true})
    ->Args({50, 5, false})
    ->Args({50, 5, true})
    ->ArgNames({"str len", "vec size", "equal"});
}

template<typename F>
static void BM_NotEqualImpl(benchmark::State& state, F funcCmp) {
    const size_t strLen = state.range(0);
    const size_t vecSize = state.range(1);
    const bool same = state.range(2);

    auto input = genInputForNotEqual(strLen, vecSize);
    auto other = genInputForCmpNotEqual(input, same);

    bool notEqual {};
    for (auto _ : state) {
        for(size_t i = 0; i < input.size(); ++i) {
            notEqual = funcCmp(input[i], other[i]);
        }
        benchmark::DoNotOptimize(notEqual);
    }
    state.counters["Result"] = notEqual;
}

static void BM_NotEqualStandard(benchmark::State& state) {
    BM_NotEqualImpl(state, [](const SomeArgs& lhs, const SomeArgs& rhs) {
        return lhs.notEqual(rhs);
    });
}
BENCHMARK(BM_NotEqualStandard)->Apply(genArgumentsForNotEqual);

static void BM_NotEqualHash(benchmark::State& state) {
    BM_NotEqualImpl(state, [](const SomeArgs& lhs, const SomeArgs& rhs) {
        return lhs.notEqualByHash(rhs);
    });
}
BENCHMARK(BM_NotEqualHash)->Apply(genArgumentsForNotEqual);

static void BM_NotEqualHashCached(benchmark::State& state) {
    BM_NotEqualImpl(state, [](const SomeArgs& lhs, const SomeArgs& rhs) {
        return lhs.notEqualByHashCached(rhs);
    });
}
BENCHMARK(BM_NotEqualHashCached)->Apply(genArgumentsForNotEqual);

// Run the benchmarks
BENCHMARK_MAIN();
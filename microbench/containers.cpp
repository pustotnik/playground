
#include <cstddef>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

#include <benchmark/benchmark.h>

using namespace std;

static constexpr size_t SMALL_CONTAINER_SIZE = 20;
using StrItem = pair<std::string, size_t>;

static std::vector<StrItem> genSmallInput() {
    std::vector<StrItem> result;
    for(size_t i = 0; i < SMALL_CONTAINER_SIZE; ++i) {
        char ch = 'a' + i;
        std::string key(15, ch);
        result.push_back(StrItem(key, i + 101));
    }

    return result;
}

static const std::vector<StrItem> smallInput = genSmallInput();

static void BM_SearchInVector(benchmark::State& state) {
    std::vector<StrItem> container = smallInput;
    auto findStr = smallInput[smallInput.size() - 1].first;

    int found = 0;
    for (auto _ : state) {
        for(size_t i = 0; i < smallInput.size(); ++i) {
            if(findStr == container[i].first) {
                found = container[i].second;
                break;
            }
        }
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInVector);

static void BM_SearchInMap(benchmark::State& state) {
    std::map<std::string, size_t> container;
    for(auto [key, val]: smallInput) {
        container[key] = val;
    }

    auto findStr = smallInput[smallInput.size() - 1].first;
    int found = 0;
    for (auto _ : state) {
        found = container.find(findStr)->second;
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInMap);

static void BM_SearchInUnorderedMap(benchmark::State& state) {
    std::unordered_map<std::string, size_t> container;
    for(auto [key, val]: smallInput) {
        container[key] = val;
    }
    auto findStr = smallInput[smallInput.size() - 1].first;

    int found = 0;
    for (auto _ : state) {
        found = container.find(findStr)->second;
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInUnorderedMap);


// Run the benchmarks
BENCHMARK_MAIN();
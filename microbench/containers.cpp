
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

#include <benchmark/benchmark.h>

using namespace std;

namespace {

constexpr size_t SMALL_CONTAINER_SIZE = 20;
constexpr const char CHARSET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

using StrItem = pair<std::string, size_t>;

std::string gen_random_str() {
    std::string str(CHARSET, 18);
    for(size_t i = 0; i < str.size(); ++i) {
        swap(str[i], str[rand() % str.size()]);
    }
    return str;
}

std::vector<StrItem> genInput(size_t size) {
    std::vector<StrItem> result;
    // we want random strings but we want the same random strings for every benchmark
    srand(11);
    for(size_t i = 0; i < size; ++i) {
        result.push_back(StrItem(gen_random_str(), i + 101));
    }

    return result;
}

const std::vector<StrItem> smallInput = genInput(SMALL_CONTAINER_SIZE);

} // namespace

static void genArguments(benchmark::internal::Benchmark* b) {
    b
    ->Args({0,})
    ->Args({static_cast<long>(smallInput.size()/2), })
    ->Args({static_cast<long>(smallInput.size()-1), })
    ->ArgNames({"search position", });
}

static void BM_SearchInVector(benchmark::State& state) {
    const size_t pos = state.range(0);

    std::vector<StrItem> container = smallInput;
    auto findStr = smallInput[pos].first;

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
BENCHMARK(BM_SearchInVector)->Apply(genArguments);

static void BM_SearchInMap(benchmark::State& state) {
    const size_t pos = state.range(0);

    std::map<std::string, size_t> container;
    for(auto [key, val]: smallInput) {
        container[key] = val;
    }

    auto findStr = smallInput[pos].first;
    int found = 0;
    for (auto _ : state) {
        found = container.find(findStr)->second;
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInMap)->Apply(genArguments);

static void BM_SearchInUnorderedMap(benchmark::State& state) {
    const size_t pos = state.range(0);

    std::unordered_map<std::string, size_t> container;
    for(auto [key, val]: smallInput) {
        container[key] = val;
    }

    auto findStr = smallInput[pos].first;
    int found = 0;
    for (auto _ : state) {
        found = container.find(findStr)->second;
        benchmark::DoNotOptimize(found);
    }
    state.counters["Result"] = found;
}
BENCHMARK(BM_SearchInUnorderedMap)->Apply(genArguments);


// Run the benchmarks
BENCHMARK_MAIN();

#include <cstddef>
#include <tuple>
#include <utility>

#include <benchmark/benchmark.h>

using namespace std;

struct Test
{
    struct Line {
        const char* data = nullptr;
        size_t      len = 0;
    };

    const char* test1(size_t& len);

    Line test2();

    std::pair<const char*, size_t> test3();

    std::tuple<const char*, size_t> test4();

    char data[10] = {0};
    size_t size = 10;
};

const char* Test::test1(size_t& len) {
    len = size;
    const char* p = data;
    return p;
}

Test::Line Test::test2() {
    Line result;
    result.data = data;
    result.len = size;
    return result;
}

std::pair<const char*, size_t> Test::test3() {
    return {data, size};
}

std::tuple<const char*, size_t> Test::test4() {
    return {data, size};
}

static void returnAsClassic(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        size_t len;
        auto result = test.test1(len);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(returnAsClassic);

static void returnAsStruct(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto [data, len] = test.test2();
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(returnAsStruct);

static void returnAsPair(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto [data, len] = test.test3();
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(returnAsPair);

static void returnAsTuple(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto [data, len] = test.test4();
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(returnAsTuple);

// Run the benchmarks
BENCHMARK_MAIN();
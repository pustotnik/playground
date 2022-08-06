
#include <cstddef>
#include <tuple>
#include <utility>
#include <iostream>

#include <benchmark/benchmark.h>

using namespace std;

static constexpr size_t VECT_SIZE = 100;

class Test
{
public:
    struct Line {
        const char* data = nullptr;
        size_t      len = 0;
    };

    Test(): _lines(VECT_SIZE) {
        for(size_t i = 0; i < _lines.size(); ++i) {
            _lines[i].len = i;
        }
    }

    const char* returnOneAndArg(size_t& len);

    Line returnStruct();

    std::pair<const char*, size_t> returnPair();

    std::tuple<const char*, size_t> returnTuple();

    void returnVectorAsArg(vector<Line>& v);
    vector<Line> returnVector();

private:
    char _data[10] = {0};
    size_t _size = 10;

    vector<Line> _lines;
};

const char* Test::returnOneAndArg(size_t& len) {
    len = _size;
    const char* p = _data;
    return p;
}

Test::Line Test::returnStruct() {
    Line result;
    result.data = _data;
    result.len = _size;
    return result;
}

std::pair<const char*, size_t> Test::returnPair() {
    return {_data, _size};
}

std::tuple<const char*, size_t> Test::returnTuple() {
    return {_data, _size};
}

void Test::returnVectorAsArg(vector<Line>& v) {
    v = _lines;
}

vector<Test::Line> Test::returnVector() {
    return _lines;
}

static void BM_ReturnAsClassic(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        size_t len;
        auto result = test.returnOneAndArg(len);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(BM_ReturnAsClassic);

static void BM_ReturnAsStruct(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto [data, len] = test.returnStruct();
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(BM_ReturnAsStruct);

static void BM_ReturnAsPair(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto [data, len] = test.returnPair();
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(BM_ReturnAsPair);

static void BM_ReturnAsTuple(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto [data, len] = test.returnTuple();
        benchmark::DoNotOptimize(data);
        benchmark::DoNotOptimize(len);
    }
}
BENCHMARK(BM_ReturnAsTuple);

static void BM_ReturnVectorAsArg(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        vector<Test::Line> v;
        test.returnVectorAsArg(v);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_ReturnVectorAsArg);

static void BM_ReturnVector(benchmark::State& state) {
    Test test;
    for (auto _ : state) {
        auto v = test.returnVector();
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_ReturnVector);

static void BM_CopyVector(benchmark::State& state) {

    vector<Test::Line> lines(VECT_SIZE);
    for(size_t i = 0; i < lines.size(); ++i) {
        lines[i].len = i*2;
    }

    for (auto _ : state) {
        auto v = lines;
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_CopyVector);

// Run the benchmarks
BENCHMARK_MAIN();
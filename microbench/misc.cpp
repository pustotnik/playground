
#include <functional>
#include <string>

#include <benchmark/benchmark.h>

using namespace std;

//////
// Misc bencmarks


struct TestFunc
{
    std::string str = "test1";
    // const int iters = 1024*1024*1024;
    const int iters = 1024;

    // passing std::fuction as rvalue speeds up it a little, but not much:
    // as lvalue: 2408 ns
    // as rvalue: 2127 ns / 2067 ns
    int runStdFunc(std::function<int (int n, const std::string& str)>&& func) {
        int res = 0;
        for (int i = 0; i < iters; ++i) {
            res += func(i, str);
        }
        return res;
    }

    template <typename Func>
    int runLambda(Func&& func) {
        int res = 0;
        for (int i = 0; i < iters; ++i) {
            res += func(i, str);
        }
        return res;
    }
};

static void BM_UseStdFunc(benchmark::State& state) {
    TestFunc test;
    int dep1 = 100;
    int dep2 = 200;
    int result = 0;
    for (auto _ : state) {
        result = test.runStdFunc([&dep1, &dep2](int n, const std::string& str){
            return str.empty() ? n + dep1 : n + dep2;
        });
        benchmark::DoNotOptimize(result);
    }
    state.counters["Result"] = result;
}
BENCHMARK(BM_UseStdFunc);

static void BM_UseLambda(benchmark::State& state) {
    TestFunc test;
    int dep1 = 100;
    int dep2 = 200;
    int result = 0;
    for (auto _ : state) {
        result = test.runLambda([&dep1, &dep2](int n, const std::string& str){
            return str.empty() ? n + dep1 : n + dep2;
        });
        benchmark::DoNotOptimize(result);
    }
    state.counters["Result"] = result;
}
BENCHMARK(BM_UseLambda);

static void BM_UseAsIsWithoutFunc(benchmark::State& state) {
    TestFunc test;
    int dep1 = 100;
    int dep2 = 200;
    int result = 0;
    for (auto _ : state) {
        int res = 0;
        for (int i = 0; i < test.iters; ++i) {
            res += test.str.empty() ? i + dep1 : i + dep2;
        }
        result = res;
        benchmark::DoNotOptimize(result);
    }
    state.counters["Result"] = result;
}
BENCHMARK(BM_UseAsIsWithoutFunc);

// Run the benchmarks
BENCHMARK_MAIN();
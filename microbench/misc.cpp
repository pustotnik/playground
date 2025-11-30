
#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>
#include <string>
#include <iostream>

#include <benchmark/benchmark.h>

using namespace std;

//////
// Misc bencmarks


struct TestFunc {
    string str = "test1";
    // const int iters = 1024*1024*1024;
    const int iters = 1024;

    // passing std::fuction as rvalue speeds up it a little, but not much:
    // as lvalue: 2408 ns
    // as rvalue: 2127 ns / 2067 ns
    int runStdFunc(std::function<int (int n, const string& str)>&& func) {
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

struct TestReverse {

    TestReverse(): result_strs(strs) {
    };

    const vector<string> strs = {
        "", "test", "some long long long string",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ...",
    };

    vector<string> result_strs;

    string byStdReverseConst(const string& str) {
        string result(str);
        std::reverse(result.begin(), result.end());
        return result;
    }

    void byStdReverse(string& str) {
        std::reverse(str.begin(), str.end());
    }

    string byContructor(const string& str) {
        // It works faster than byForLoopConst and byForLoopConst2
        return string(str.rbegin(), str.rend());
    }

    std::string byForLoopConst(const string& str) {
        string result;
        if (str.empty()) {
            return result;
        }
        result.reserve(str.size());
        for (int i = str.size() - 1; i >= 0; --i) {
            result.push_back(str[i]);
        }
        // for (auto it = str.rbegin(); it != str.rend(); ++it) {
        //     result.push_back(*it);
        // }
        return result;
    }

    std::string byForLoopConst2(const string& str) {
        string result(str);
        if (str.empty()) {
            return result;
        }
        auto middle = result.size() / 2;
        auto last_idx = result.size() - 1;
        for (size_t i = 0; i < middle; ++i) {
            std::swap(result[i], result[last_idx - i]);
        }
        return result;
    }

    void byForLoop(string& str) {
        if (str.empty()) {
            return;
        }
        auto middle = str.size() / 2;
        auto last_idx = str.size() - 1;
        for (size_t i = 0; i < middle; ++i) {
            std::swap(str[i], str[last_idx - i]);
        }
    }

    void byForLoop2(string& str) {
        auto first = str.begin();
        auto last = str.end();
        if (first == last) {
            return;
        }
        --last;
        while(first < last) {
            std::iter_swap(first, last);
            // std::swap(*first, *last);
            ++first;
            --last;
        }
    }

    template <typename Func>
    void runConst(Func&& func) {
        result_strs = strs;
        for (size_t i = 0; i < strs.size(); ++i) {
            result_strs[i] = func(strs[i]);
        }
    }

    template <typename Func>
    void run(Func&& func) {
        result_strs = strs;
        for (size_t i = 0; i < strs.size(); ++i) {
            func(result_strs[i]);
        }
    }

    void printResultStrs() {
        std::for_each(result_strs.begin(), result_strs.end(), [](const string& s) {
            cout << "\"" << s << "\", ";
        });
        cout << endl;
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

static void BM_ReverseStr_StdReverseConst(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.runConst([&](const string &str){ return test.byStdReverseConst(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_StdReverseConst);

static void BM_ReverseStr_ContructorConst(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.runConst([&](const string &str){ return test.byContructor(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_ContructorConst);

static void BM_ReverseStr_ForLoopConst(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.runConst([&](const string &str){ return test.byForLoopConst(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_ForLoopConst);

static void BM_ReverseStr_ForLoopConst2(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.runConst([&](const string &str){ return test.byForLoopConst2(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_ForLoopConst2);

static void BM_ReverseStr_StdReverse(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.run([&](string &str){ test.byStdReverse(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_StdReverse);

static void BM_ReverseStr_ForLoop(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.run([&](string &str){ test.byForLoop(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_ForLoop);

static void BM_ReverseStr_ForLoop2(benchmark::State& state) {
    TestReverse test;
    std::string result;
    for (auto _ : state) {
        test.run([&](string &str){ test.byForLoop2(str);});
        benchmark::DoNotOptimize(test.result_strs);
    }

    // test.printResultStrs();
}
BENCHMARK(BM_ReverseStr_ForLoop2);


// Run the benchmarks
BENCHMARK_MAIN();

#include <cstddef>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>

#include <benchmark/benchmark.h>

using namespace std;

static const int MAX_NUM_OF_THREADS = std::thread::hardware_concurrency();

typedef vector<int> Matrix;

static void initMatrix(Matrix& m, size_t dim) {

    m.resize(dim * dim);

    Matrix::value_type v = 0;
    for(auto &elem: m) {
        elem = ++v;
    }
}

static int calcInBadWay(Matrix& matrix, const size_t dim, const size_t numOfThreads) {

    vector<int> result(numOfThreads);
    vector<thread> pool(numOfThreads);

    for(size_t p = 0; p < numOfThreads; ++p) {
        pool[p] = thread(
            [&, p]{
                result[p] = 0;
                auto chunkSize = dim/numOfThreads + 1;
                auto myStart = p * chunkSize;
                auto myEnd = min( myStart + chunkSize, dim );
                for(size_t i = myStart; i < myEnd; ++i) {
                    for(size_t j = 0; j < dim; ++j) {
                        if(matrix[i * dim + j] % 2)
                            ++result[p];
                    }
                }
            }
        );
    }

    for(auto &t: pool) {
        t.join();
    }

    int odds = 0; // combine the results
    for(auto n: result) {
        odds += n;
    }

    return odds;
}

static int calcInGoodWay(Matrix& matrix, const size_t dim, const size_t numOfThreads) {

    vector<int> result(numOfThreads);
    vector<thread> pool(numOfThreads);

    for(size_t p = 0; p < numOfThreads; ++p) {
        pool[p] = thread(
            [&, p]{
                int count = 0;
                auto chunkSize = dim/numOfThreads + 1;
                auto myStart = p * chunkSize;
                auto myEnd = min( myStart + chunkSize, dim );
                for(size_t i = myStart; i < myEnd; ++i) {
                    for(size_t j = 0; j < dim; ++j) {
                        if(matrix[i * dim + j] % 2)
                            ++count;
                    }
                }

                result[p] = count;
            }
        );
    }

    for(auto &t: pool) {
        t.join();
    }

    int odds = 0; // combine the results
    for(auto n: result) {
        odds += n;
    }

    return odds;
}

static void BM_Scalability(benchmark::State& state) {

    const size_t numOfThreads = state.range(0);
    const size_t dim          = state.range(2);
    const bool inGoodWay      = state.range(1);

    Matrix m;
    initMatrix(m, dim);

    int result = 0;
    if(inGoodWay) {
        for (auto _ : state) {
            result = calcInGoodWay(m, dim, numOfThreads);
            benchmark::DoNotOptimize(result);
        }
    }
    else {
        for (auto _ : state) {
            result = calcInBadWay(m, dim, numOfThreads);
            benchmark::DoNotOptimize(result);
        }
    }

    state.counters["Result"] = result;
}

BENCHMARK(BM_Scalability)
    ->ArgsProduct({
        // num of threads
        benchmark::CreateDenseRange(1, MAX_NUM_OF_THREADS, /*step=*/1),
        // bad/good way
        benchmark::CreateDenseRange(0, 1, /*step=*/1),
        // dimension of matrix
        //benchmark::CreateRange(128, 8<<10, /*multi=*/2),
        benchmark::CreateRange(4<<10, 8<<10, /*multi=*/2),
    })
    ->ArgNames({"threads", "good?", "dim"})
    ->UseRealTime();
    //->MeasureProcessCPUTime()->UseRealTime();

// Run the benchmarks
BENCHMARK_MAIN();



#include <string>
#include <vector>
#include <iostream>

#include <benchmark/benchmark.h>

using namespace std;

// Simple implemention of a matrix class
template<typename T>
class Matrix final {
public:

    typedef T             Value;
    typedef vector<Value> Row;

    Matrix(size_t rows, size_t columns):
        _rows(rows), _cols(columns), _data(rows, Row(columns)) {
    }

    size_t rows() const { return _rows; }
    size_t columns() const { return _cols; }

    Row& operator[](size_t i) { return _data[i]; }
    const Row& operator[](size_t i) const { return _data[i]; }

private:
    typedef vector<vector<T>> Data;

    size_t _rows = 0;
    size_t _cols = 0;
    Data   _data;
};

template<typename T>
static void initMatrix(Matrix<T>& m) {

    typename Matrix<T>::Value v = 0;
    for(size_t i = 0; i < m.rows(); ++i) {
        for(size_t j = 0; j < m.columns(); ++j) {
            m[i][j] = ++v;
        }
    }
}

template<typename T>
static void sumMatrixRowMajor(Matrix<T>& m, T& sum) {

    sum = 0;
    for(size_t i = 0; i < m.rows(); ++i) {
        for(size_t j = 0; j < m.columns(); ++j) {
            sum += m[i][j];
        }
    }
}

template<typename T>
static void sumMatrixColumnMajor(Matrix<T>& m, T& sum) {

    sum = 0;
    for(size_t j = 0; j < m.columns(); ++j) {
        for(size_t i = 0; i < m.rows(); ++i) {
            sum += m[i][j];
        }
    }
}

static void BM_Traverse(benchmark::State& state) {

    const size_t rows   = state.range(0);
    const size_t cols   = state.range(1);
    const bool rawMajor = state.range(2);

    Matrix<int> m(rows, cols);
    initMatrix(m);

    int sum = 0;
    if(rawMajor) {
        for (auto _ : state) {
            sumMatrixRowMajor(m, sum);
            benchmark::DoNotOptimize(sum);
        }
    }
    else {
        for (auto _ : state) {
            sumMatrixColumnMajor(m, sum);
            benchmark::DoNotOptimize(sum);
        }
    }

    state.counters["Result"] = sum;
}

BENCHMARK(BM_Traverse)
    ->Args({8, 8, true})
    ->Args({8, 8, false})
    ->Args({28, 28, true})
    ->Args({28, 28, false})
    ->Args({128, 128, true})
    ->Args({128, 128, false})
    ->Args({4<<10, 4<<10, true})
    ->Args({4<<10, 4<<10, false})
    ->Args({8<<10, 8<<10, true})
    ->Args({8<<10, 8<<10, false});

// Run the benchmarks
BENCHMARK_MAIN();


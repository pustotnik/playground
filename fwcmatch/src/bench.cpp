
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>

#include <benchmark/benchmark.h>

#include "fgetsreader.h"
#include "mmapreader.h"
#include "fstreamreader.h"
#include "mywildcard.h"
#include "fnmatchwildcard.h"
#include "regexwildcard.h"
#include "seqproc.h"
#include "mtcondvarproc.h"
#include "mtcondvarproc2.h"
#include "mtlockfreeproc.h"
#include "mtsemproc.h"
#include "mtmpmcproc.h"
#include "mtlockreadproc.h"

using namespace fwc;

static std::string benchFileName;
static std::string benchPattern;

template<typename FReader, typename WildcardMatch>
void BM_Sequential(benchmark::State& state) {

    const size_t maxLines = state.range(0);

    auto freader   = FReader();
    auto wcmatch   = WildcardMatch();
    auto processor = SequentialProcessor(maxLines, freader.needsBuffer());

    size_t found = 0;
    for (auto _ : state) {
        found = processor.execute(freader, benchFileName, wcmatch, benchPattern);
        benchmark::DoNotOptimize(found);
    }

    state.counters["Count"] = found;
}

static void genSequentialArguments(benchmark::internal::Benchmark* b) {
    b
    ->Arg(1)
    ->Arg(4)
    ->Arg(16)
    ->Arg(32)
    ->ArgNames({"mlines", })
    //->Iterations(2)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();
}

BENCHMARK(BM_Sequential<FGetsReader, MyWildcardMatch>)
    ->Apply(genSequentialArguments);

BENCHMARK(BM_Sequential<FStreamReader, MyWildcardMatch>)
    ->Apply(genSequentialArguments);

BENCHMARK(BM_Sequential<MMapReader, MyWildcardMatch>)
    ->Apply(genSequentialArguments);

///////////////////////////////////////////////////////////

BENCHMARK(BM_Sequential<FGetsReader, FNMatch>)
    ->Apply(genSequentialArguments);

BENCHMARK(BM_Sequential<FStreamReader, FNMatch>)
    ->Apply(genSequentialArguments);

BENCHMARK(BM_Sequential<MMapReader, FNMatch>)
    ->Apply(genSequentialArguments);

///////////////////////////////////////////////////////////
// C++ std::regex is very slow
/*
BENCHMARK(BM_Sequential<FGetsReader, REMatch>)
    ->Apply(genSequentialArguments);

BENCHMARK(BM_Sequential<FStreamReader, REMatch>)
    ->Apply(genSequentialArguments);

BENCHMARK(BM_Sequential<MMapReader, REMatch>)
    ->Apply(genSequentialArguments);
//*/

template<typename Processor, typename FReader, typename WildcardMatch>
void MTProdConsTempl(benchmark::State& state) {

    const size_t queueSize     = state.range(0);
    const size_t numOfThreads  = state.range(1);
    const size_t maxLines      = state.range(2);

    assert(queueSize > 0);
    assert(numOfThreads > 1);
    assert(maxLines > 0);

    auto freader   = FReader();
    auto wcmatch   = WildcardMatch();
    auto processor = Processor(queueSize, numOfThreads - 1,
                                    maxLines, freader.needsBuffer());

    size_t found = 0;
    for (auto _ : state) {
        found = processor.execute(freader, benchFileName, wcmatch, benchPattern);
        benchmark::DoNotOptimize(found);
    }

    state.counters["Count"] = found;
}

template<typename FReader, typename WildcardMatch>
void BM_MTCondVar(benchmark::State& state) {
    MTProdConsTempl<MTCondVarProcessor, FReader, WildcardMatch>(state);
}

template<typename FReader, typename WildcardMatch>
void BM_MTCondVar2(benchmark::State& state) {
    MTProdConsTempl<MTCondVarProcessor2, FReader, WildcardMatch>(state);
}

template<typename FReader, typename WildcardMatch>
void BM_MTLockFree(benchmark::State& state) {
    MTProdConsTempl<MTLockFreeProcessor, FReader, WildcardMatch>(state);
}

template<typename FReader, typename WildcardMatch>
void BM_MTSem(benchmark::State& state) {
    MTProdConsTempl<MTSemProcessor, FReader, WildcardMatch>(state);
}

template<typename FReader, typename WildcardMatch>
void BM_MTMPMC(benchmark::State& state) {
    MTProdConsTempl<MPMCProcessor, FReader, WildcardMatch>(state);
}

static void genMultithreadingArguments(benchmark::internal::Benchmark* b) {
    b
    // queueSize, numOfThreads, maxLines

    ->Args({2,   2, 16})
    ->Args({2,   2, 96})
    ->Args({8,   2, 96})
    ->Args({32,  2, 96})

    ->Args({2,   4, 96})
    ->Args({4,   4, 96})
    ->Args({8,   4, 96})
    ->Args({16,  4, 96})
    ->Args({32,  4, 96})
    ->Args({128, 4, 96})
    ->Args({4,   4, 256})
    ->Args({8,   4, 256})
    ->Args({16,  4, 256})

    ->Args({8,   8, 256})
    ->Args({16,  8, 256})
    //->Args({16,  8, 512})

    ->ArgNames({"qsize", "threads", "mlines" })
    //->Iterations(2)
    ->Unit(benchmark::kMillisecond)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
}

BENCHMARK(BM_MTCondVar<FGetsReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTCondVar<FStreamReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTCondVar<MMapReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTCondVar2<FGetsReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTCondVar2<FStreamReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTCondVar2<MMapReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTLockFree<FGetsReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTLockFree<FStreamReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTLockFree<MMapReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTSem<FGetsReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTSem<FStreamReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTSem<MMapReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTMPMC<FGetsReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTMPMC<FStreamReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

BENCHMARK(BM_MTMPMC<MMapReader, MyWildcardMatch>)
    ->Apply(genMultithreadingArguments);

template<typename FReader, typename WildcardMatch>
void BM_MTLockRead(benchmark::State& state) {

    const size_t numOfThreads  = state.range(0);
    const size_t maxLines      = state.range(1);

    auto freader   = FReader();
    auto wcmatch   = WildcardMatch();
    auto processor = MTLockReadProcessor(numOfThreads, maxLines, freader.needsBuffer());

    size_t found = 0;
    for (auto _ : state) {
        found = processor.execute(freader, benchFileName, wcmatch, benchPattern);
        benchmark::DoNotOptimize(found);
    }

    state.counters["Count"] = found;
}

static void genMultithreading2Arguments(benchmark::internal::Benchmark* b) {
    b
    // numOfThreads, maxLines

    ->Args({2, 96})
    ->Args({2, 256})
    ->Args({2, 512})

    ->Args({4, 96})
    ->Args({4, 256})
    ->Args({4, 512})

    ->Args({8, 96})
    ->Args({8, 256})
    ->Args({8, 512})

    ->ArgNames({"threads", "mlines" })
    ->Unit(benchmark::kMillisecond)
    ->MeasureProcessCPUTime()
    ->UseRealTime();
}

/*
BENCHMARK(BM_MTLockRead<FGetsReader, MyWildcardMatch>)
    ->Apply(genMultithreading2Arguments);
*/

BENCHMARK(BM_MTLockRead<FStreamReader, MyWildcardMatch>)
    ->Apply(genMultithreading2Arguments);

BENCHMARK(BM_MTLockRead<MMapReader, MyWildcardMatch>)
    ->Apply(genMultithreading2Arguments);

static bool handleEnvVars() {

    const char* envvar = nullptr;

    auto printErr = [](const std::string& msg) {
        std::cerr << msg << std::endl;
    };

    envvar = std::getenv("BENCH_FILENAME");
    if(!envvar) {
        printErr("Environment variable BENCH_FILENAME is not set!");
        return false;
    }
    benchFileName = envvar;
    if(benchFileName.empty()) {
        printErr("Environment variable BENCH_FILENAME is empty!");
        return false;
    }

    envvar = std::getenv("BENCH_PATTERN");
    if(!envvar) {
        printErr("Environment variable BENCH_PATTERN is not set!");
        return false;
    }
    benchPattern = envvar;

    return true;
}

// Run the benchmarks
int main(int argc, char** argv) {

    // Unfortunately google benchmark library does not support custom
    // arguments in a command line so I decided to use env vars.

    if(!handleEnvVars()) {
        return 1;
    }

    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }

    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
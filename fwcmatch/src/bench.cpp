
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
#include "common.h"

using namespace std;

static string benchFileName;
static string benchPattern;

template<typename FReader, typename WildcardMatch>
void BM_Sequential(benchmark::State& state) {

    const size_t maxLines = state.range(0);

    auto freader   = FReader();
    auto wcmatch   = WildcardMatch();
    auto processor = SequentialProcessor(maxLines);

    size_t found = 0;
    for (auto _ : state) {
        found = processor.execute(benchFileName, benchPattern, wcmatch, freader);
        benchmark::DoNotOptimize(found);
    }

    state.counters["Count"] = found;
}

static void genSequentialArguments(benchmark::internal::Benchmark* b) {
    b->Arg(1)->Arg(2)->Arg(4)->Arg(20)->Arg(40)
    ->ArgNames({"mlines", })
    ->Iterations(2)
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

// Run the benchmarks
int main(int argc, char** argv) {

    // Unfortunately google benchmark library does not support custom
    // arguments in a command line so I decided to use env vars.

    const char* envvar = std::getenv("BENCH_FILENAME");
    benchFileName = envvar ? envvar : "";
    envvar = std::getenv("BENCH_PATTERN");
    benchPattern = envvar ? envvar : "";

    bool noEnvVars = false;
    if(benchFileName.empty()) {
        cout << "Environment variable BENCH_FILENAME is not set or empty!" << endl;
        noEnvVars = true;
    }

    if(benchPattern.empty()) {
        cout << "Environment variable BENCH_PATTERN is not set or empty!" << endl;
        noEnvVars = true;
    }

    if(noEnvVars) {
        return 1;
    }

    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
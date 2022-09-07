
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <numeric>
#include <thread>
#include <omp.h>

#include "proctools.h"
#include "mtlockreadproc.h"

// I don't know why but implementation with OpenMP can work a little bit faster
#define USE_OPENMP_IMPL 1

namespace fwc {

static constexpr size_t BLOCK_SIZE = 2*1024;

MTLockReadProcessor::MTLockReadProcessor(size_t numOfThreads, size_t maxLines, bool needsBuffer):
    _counters(numOfThreads, 0),
    _numOfThreads(numOfThreads) {

    assert(maxLines > 0);
    assert(_numOfThreads > 0);

    _linesBlocks.reserve(_numOfThreads);
    for(size_t i = 0; i < _numOfThreads; ++i) {
        _linesBlocks.emplace_back(maxLines, needsBuffer, BLOCK_SIZE);
    }
}

size_t MTLockReadProcessor::execute(FileReader& freader, const std::string& filename,
                            WildcardMatch& wcmatch, const std::string& pattern) {

    ScopedFileOpener fopener(freader, filename);

#if ! USE_OPENMP_IMPL
    auto threadFunc = [&](size_t idx) {
        size_t result = 0;
        auto& block = _linesBlocks[idx];

        for(;;) {
            {
                // Usually it isn't the best idea to make mutex lock on slow
                // operations but here it is specific for this way
                // of solving the problem.
                std::scoped_lock lock(_mutex);
                proctools::readInLinesBlock(freader, block);
            }

            if(block.lines().empty()) {
                // end of file
                break;
            }

            result += proctools::filterBlock(wcmatch, pattern, block);
        }

        _counters[idx] = result;
    };

    std::vector<std::thread> threads;
    threads.reserve(_numOfThreads - 1);
    for(size_t i = 0; i < _numOfThreads - 1; ++i) {
        threads.emplace_back(threadFunc, i);
    }

    threadFunc(_numOfThreads - 1);

    for(auto& t: threads) {
        t.join();
    }

    return std::accumulate(_counters.begin(), _counters.end(),
                                decltype(_counters)::value_type(0));

#else
    size_t result = 0;

    #pragma omp parallel num_threads(_numOfThreads) \
            shared(freader, wcmatch, pattern, _linesBlocks) \
            reduction(+:result)
    for(;;) {

        auto idx = omp_get_thread_num();
        auto& block = _linesBlocks[idx];

        #pragma omp critical
        { proctools::readInLinesBlock(freader, block); }

        if(block.lines().empty()) {
            // end of file
            break;
        }

        result += proctools::filterBlock(wcmatch, pattern, block);
    }

    return result;

#endif
}

} // namespace fwc
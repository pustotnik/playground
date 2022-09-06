
#include <cassert>
#include <algorithm>

#include "basepcproc.h"

#define PRODUCER_HAS_OWN_THREAD 0 // just for some experiment

namespace fwc {

BaseProdConsProcessor::BaseProdConsProcessor(size_t numOfConsumers):
    _counters(numOfConsumers, 0),
    _numOfConsThreads(numOfConsumers) {

    assert(numOfConsumers > 0);
}

BaseProdConsProcessor::~BaseProdConsProcessor() {
}

size_t BaseProdConsProcessor::execute(FileReader& freader, const std::string& filename,
                                WildcardMatch& wcmatch, const std::string& pattern) {

    // std::fill works slowly :(
    _counters.assign(_counters.size(), 0);

    init();
    ScopedFileOpener fopener(freader, filename);

    std::vector<std::thread> threads;
    threads.reserve(_numOfConsThreads + 1);
    for(size_t i = 0; i < _numOfConsThreads; ++i) {
        threads.emplace_back(&BaseProdConsProcessor::filterLines,
                            this, i, std::ref(wcmatch), std::cref(pattern));
    }

#if PRODUCER_HAS_OWN_THREAD
    threads.emplace_back(&BaseMTProcessor::readFileLines, this, std::ref(freader));
#else
    readFileLines(freader);
#endif

    for(auto& t: threads) {
        t.join();
    }

    return calcFinalResult();
}

size_t BaseProdConsProcessor::calcFinalResult() const {

    return std::accumulate(_counters.begin(), _counters.end(),
                                decltype(_counters)::value_type(0));
}

} // namespace fwc
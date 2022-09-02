
#include <cassert>
#include <algorithm>

#include "basemtproc.h"

using namespace std;

#define PRODUCER_HAS_OWN_THREAD 0 // just for some experiment

BaseMTProcessor::BaseMTProcessor(size_t numOfConsThreads, size_t maxLines):
    _counters(numOfConsThreads, 0),
    _numOfConsThreads(numOfConsThreads),
    _maxLines(maxLines) {

    assert(numOfConsThreads > 0);
    assert(maxLines > 0);
}

BaseMTProcessor::~BaseMTProcessor() {
}

size_t BaseMTProcessor::execute(FileReader& freader, const string& filename,
                                WildcardMatch& wcmatch, const string& pattern) {

    // std::fill works slowly :(
    _counters.assign(_counters.size(), 0);

    init();
    ScopedFileOpener fopener(freader, filename);

    Threads threads;
    threads.reserve(_numOfConsThreads + 1);
    for(size_t i = 0; i < _numOfConsThreads; ++i) {
        threads.emplace_back(&BaseMTProcessor::filterLines,
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

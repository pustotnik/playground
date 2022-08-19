
#include <cassert>
#include <algorithm>

#include "basemtproc.h"

using namespace std;

BaseMTProcessor::BaseMTProcessor(size_t numOfConsThreads, size_t maxLines):
    _maxLines(maxLines) {

    assert(numOfConsThreads > 0);
    assert(maxLines > 0);

    _threads.resize(numOfConsThreads + 1);
}

BaseMTProcessor::~BaseMTProcessor() {
}

size_t BaseMTProcessor::execute(FileReader& freader, const string& filename,
                                WildcardMatch& wcmatch, const string& pattern) {

    init(freader.needsBuffer());
    ScopedFileOpener fopener(freader, filename);

    _threads[0] = thread(&BaseMTProcessor::readFileLines,
                                            this, std::ref(freader));

    for(size_t i = 1; i < _threads.size(); ++i) {
        _threads[i] = thread(&BaseMTProcessor::filterLines,
                            this, i-1, std::ref(wcmatch), std::cref(pattern));
    }

    for(auto& t: _threads) {
        t.join();
    }

    return calcFinalResult();
}

#pragma once

#include <cstddef>
#include <string>
#include <numeric>
#include <algorithm>
#include <vector>
#include <thread>

#include "noncopyable.h"
#include "linesblock.h"
#include "wildcard.h"
#include "filereader.h"

namespace fwc {

// Base class for producer-consumer implementations
class BaseProdConsProcessor: private noncopyable
{
public:

    BaseProdConsProcessor(size_t numOfConsumers);
    virtual ~BaseProdConsProcessor();

    size_t execute(FileReader& freader, const std::string& filename,
                    WildcardMatch& wcmatch, const std::string& pattern);

protected:

    std::vector<size_t> _counters;

private:

    // it is called in the 'execute' method in the beginning (so threads haven't started yet)
    virtual void init() = 0;

    // it is called in producer thread
    virtual void readFileLines(FileReader& freader) = 0;

    // it is called in consumer threads
    virtual void filterLines(size_t idx, WildcardMatch& wcmatch, const std::string& pattern) = 0;

    // gather all counters from all consumers
    // it is called in the 'execute' method after all threads finished
    virtual size_t calcFinalResult() const;

    const size_t  _numOfConsThreads;
};

} // namespace fwc
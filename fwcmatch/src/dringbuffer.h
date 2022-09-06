#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>

#include "noncopyable.h"

namespace fwc {

// Some ring/circular buffer for single producer and many consumers
// and based on some idea from LMAX Disruptor.
// Not thread safe but can be used with mutex locks outside.
template <typename T>
class DRingBuffer final: private noncopyable {
public:
    using Value    = T;
    using uint64_t = std::uint64_t;

    constexpr static uint64_t NULL_POS = -1;

    DRingBuffer(size_t capacity, size_t maxConsumers):
        _buffer(capacity),
        _consumerHeads(maxConsumers, NULL_POS) {

        assert(capacity > 1);
        assert(maxConsumers > 0);
    }

    // reset to initial state
    // this method does not touch values in the internal buffer
    void reset() noexcept {
        _head = _fhead =_tail = 0;
        _consumerHeads.assign(_consumerHeads.size(), NULL_POS);
    }

    [[nodiscard]] Value* enqueuePrepare() {
        assert(!full());
        return &at(_tail);
    }

    void enqueueCommit() {
        ++_tail;
    }

    [[nodiscard]] Value* dequeuePrepare(size_t consumerId) {
        assert(consumerId < _consumerHeads.size());
        assert(NULL_POS == _consumerHeads[consumerId]);
        assert(!empty());

        auto* item = &at(_fhead++);
        _consumerHeads[consumerId] = _fhead;
        return item;
    }

    void dequeueCommit(size_t consumerId) {
        assert(consumerId < _consumerHeads.size());
        assert(NULL_POS != _consumerHeads[consumerId]);

        _consumerHeads[consumerId] = NULL_POS;
        auto minPos = minConsumerPos();

        assert(NULL_POS == minPos || minPos > _head);
        _head = (NULL_POS == minPos) ? _fhead : minPos - 1;
    }

    // Apply function to all values in the internal buffer
    template<typename Callable>
    void apply(Callable&& func) {
        for(auto& v: _buffer) {
            func(v);
        }
    }

    [[nodiscard]] size_t capacity() const noexcept { return _buffer.size(); }
    [[nodiscard]] size_t size() const noexcept {
        return static_cast<size_t>(_tail - _head);
    }
    [[nodiscard]] bool empty() const noexcept { return _fhead == _tail; }
    [[nodiscard]] bool full() const noexcept { return size() == capacity(); }

private:
    std::vector<T>        _buffer;
    std::vector<uint64_t> _consumerHeads;
    uint64_t              _head  { 0 };
    uint64_t              _fhead { 0 };
    uint64_t              _tail  { 0 };

    Value& at(size_t idx) noexcept {
        return _buffer[idx % capacity()];
    }

    uint64_t minConsumerPos() const noexcept {
        auto minPos = NULL_POS;
        for(auto pos: _consumerHeads) {
            minPos = std::min(minPos, pos);
        }
        return minPos;
    }
};

} // namespace fwc

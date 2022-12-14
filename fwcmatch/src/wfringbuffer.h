#pragma once

#include <cassert>
#include <cstddef>
#include <vector>
#include <atomic>

#include "noncopyable.h"

namespace fwc {

// Simple wait-free ring/circular buffer.
// Thread safety is guaranteed in this way when push() and pop()
// are used by a at most one corresponding thread.
// Based on https://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular
// It is not general-purpose implementation.
template <typename T>
class WFSimpleRingBuffer final: private noncopyable {
public:
    using Value = T;

    explicit WFSimpleRingBuffer(size_t capacity):
        _bufferHolder(capacity + 1),
        _buffer(_bufferHolder.data()),
        _capacity(capacity + 1) {

        assert(capacity > 0);
    }

    bool push(const Value& v) {
        const auto tail = _tail.load(std::memory_order_relaxed);
        const auto nextTail = increment(tail);
        if(nextTail != _head.load(std::memory_order_acquire)) {
            _buffer[tail] = v;
            _tail.store(nextTail, std::memory_order_release);
            return true;
        }

        return false; // full
    }

    template<typename Callable>
    bool push(Callable&& writer) {
        const auto tail = _tail.load(std::memory_order_relaxed);
        const auto nextTail = increment(tail);
        if(nextTail != _head.load(std::memory_order_acquire)) {
            writer(_buffer[tail]);
            _tail.store(nextTail, std::memory_order_release);
            return true;
        }

        return false; // full
    }

    bool pop(Value& v) {
        const auto head = _head.load(std::memory_order_relaxed);
        if(head != _tail.load(std::memory_order_acquire)) {
            v = _buffer[head];
            _head.store(increment(head), std::memory_order_release);
            return true;
        }

        return false; // empty
    }

    template<typename Callable>
    bool pop(Callable&& reader) {
        const auto head = _head.load(std::memory_order_relaxed);
        if(head != _tail.load(std::memory_order_acquire)) {
            auto const& v = _buffer[head];
            reader(v);
            _head.store(increment(head), std::memory_order_release);
            return true;
        }

        return false; // empty
    }

    [[nodiscard]] size_t capacity() const noexcept { return _capacity - 1; }

    // reset to initial state
    // this method does not touch values in the internal buffer
    // not thread safe
    void reset() noexcept {
        _head = 0;
        _tail = 0;
    }

    // Apply function to all values in the internal buffer
    // not thread safe
    template<typename Callable>
    void apply(Callable&& func) {
        for(auto& v: _bufferHolder) {
            func(v);
        }
    }

private:
    // I decided not to use vector directly and use additional pointer to data
    // inside that vector to avoid any hidden operations in std::vector
    // because it is more safe in terms of thread safity in this implementation.
    // C++ standard does not guarantee that implemenation of std::vector::operator[]
    // touches only internal memory buffer.
    // So vector here is only for storage with automatic deallocation.
    std::vector<T>      _bufferHolder;
    T*                  _buffer  { nullptr };
    std::atomic<size_t> _head    { 0 };
    std::atomic<size_t> _tail    { 0 };
    const size_t        _capacity;

    size_t increment(size_t idx) const noexcept {
        return (idx + 1) % _capacity;
    }
};

} // namespace fwc
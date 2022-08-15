#pragma once

#include <cassert>
#include <cstddef>
#include <vector>
#include <atomic>

#include "noncopyable.h"

// Simple wait-free ring/circular buffer.
// Thread safety is guaranteed in this way when push() and pop()
// are used by a at most one corresponding thread.
// Based on https://www.codeproject.com/Articles/43510/Lock-Free-Single-Producer-Single-Consumer-Circular
// It is not general-purpose implementation.
template <typename T>
class WFSimpleRingBuffer final: private noncopyable {
public:
    typedef T Value;

    explicit WFSimpleRingBuffer(size_t capacity):
    _buffer(capacity + 1), _capacity(capacity + 1) {
    }

    ~WFSimpleRingBuffer() {
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

    bool pop(Value& v) {
        const auto head = _head.load(std::memory_order_relaxed);
        if(head != _tail.load(std::memory_order_acquire)) {
            v = _buffer[head];
            _head.store(increment(head), std::memory_order_release);
            return true;
        }

        return false; // empty
    }

    [[nodiscard]] size_t capacity() const noexcept { return _buffer.size(); }

    // not thread safe
    void clear() noexcept {
        _head = 0;
        _tail = 0;
    }

private:
    std::vector<T>      _buffer;
    std::atomic<size_t> _head {0};
    std::atomic<size_t> _tail {0};
    const size_t        _capacity;

    size_t increment(size_t idx) const noexcept {
        return (idx + 1) % _capacity;
    }
};

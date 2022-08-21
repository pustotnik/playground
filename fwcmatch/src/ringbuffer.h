#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "noncopyable.h"

// Simple ring/circular buffer
// It is not general-purpose implementation.
template <typename T>
class SimpleRingBuffer final: private noncopyable {
public:
    using Value = T;

    explicit SimpleRingBuffer(size_t capacity): _buffer(capacity) {
    }

    ~SimpleRingBuffer() {
    }

    [[nodiscard]] size_t size() const noexcept { return _size; }
    [[nodiscard]] size_t capacity() const noexcept { return _buffer.size(); }
    [[nodiscard]] bool empty() const noexcept { return 0 == size(); }
    [[nodiscard]] bool full() const noexcept { return size() == capacity(); }

    // reset to initial state
    // this method does not touch values in the internal buffer
    void reset() noexcept {
        _head = _tail = _size = 0;
    }

    void push(const Value& v) {
        // this implementaion does not allow to replace existing item
        assert(!full());

        _buffer[_tail] = v;
        increment(_tail);
        ++_size;
    }

    // push with swapping instead of copying
    void pushSwapping(Value& v) {
        // this implementaion does not allow to replace existing item
        assert(!full());

        std::swap(_buffer[_tail], v);
        increment(_tail);
        ++_size;
    }

    void pop() {
        // this implementaion does not allow to pop not existing item
        assert(!empty());

        increment(_head);
        --_size;
    }

    [[nodiscard]] Value& top() { assert(!empty()); return _buffer[_head]; }
    [[nodiscard]] Value const& top() const { assert(!empty()); return _buffer[_head]; }

    // Apply function to all values in the internal buffer
    template<typename Callable>
    void apply(Callable&& func) {
        for(auto& v: _buffer) {
            func(v);
        }
    }

private:
    std::vector<T> _buffer;
    size_t         _head { 0 };
    size_t         _tail { 0 };
    size_t         _size { 0 };

    void increment(size_t& idx) noexcept {
        if(++idx >= capacity()) {
            idx = 0;
        }
    }
};

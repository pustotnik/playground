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
    typedef T Value;

    SimpleRingBuffer(size_t capacity): _buffer(capacity) {
    }

    ~SimpleRingBuffer() {
    }

    bool size() const { return _size; }
    bool capacity() const { return _buffer.size(); }
    bool empty() const { return 0 == size(); }
    bool full() const { return size() == capacity(); }

    void clear() {
        _head = _tail = _size = 0;
    }

    void push(const Value& v) {
        // this implementaion does not allow to replace existing item
        assert(!full());

        _buffer[_tail] = v;
        increment(_tail);
        ++_size;
    }

    void pop() {
        // this implementaion does not allow to pop not existing item
        assert(!empty());

        increment(_head);
        --_size;
    }

    Value& top() { assert(!empty()); return _buffer[_head]; }
    Value const& top() const { assert(!empty()); return _buffer[_head]; }

private:
    std::vector<T> _buffer;
    size_t         _head = 0;
    size_t         _tail = 0;
    size_t         _size = 0;

    void increment(size_t& idx) {
        if(++idx >= capacity()) {
            idx = 0;
        }
    }
};

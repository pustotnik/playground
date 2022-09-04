#pragma once

#include <cstddef>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <type_traits>

#include "ringbuffer.h"

// Simple buffer for data blocks
class BlocksBuffer final {
public:
    using Pointer      = char*;
    using ConstPointer = char const*;

    constexpr static size_t DEFAULT_BLOCK_SIZE = 2*1024;

    BlocksBuffer() noexcept = default;

    explicit BlocksBuffer(size_t size, size_t blockSize = DEFAULT_BLOCK_SIZE) {
        resize(size, blockSize);
    }

    BlocksBuffer(BlocksBuffer&& other) noexcept = default;
    BlocksBuffer& operator=(BlocksBuffer&& other) noexcept = default;

    // resize buffer,
    // there is no real memory allocation if capacity() returns enough big size
    void resize(size_t numOfBlocks, size_t blockSize = DEFAULT_BLOCK_SIZE) {
        assert(0 == numOfBlocks || (numOfBlocks > 0 && blockSize > 0));
        _blockSize = blockSize;
        _buffer.resize(_blockSize * numOfBlocks);
    }

    void swap(BlocksBuffer& other) noexcept {
        _buffer.swap(other._buffer);
        std::swap(_blockSize, other._blockSize);
    }

    void clear() noexcept { _buffer.clear(); }

    // get current buffer size
    [[nodiscard]]
    size_t size() const noexcept { return _buffer.size(); }

    // get capacity of buffer
    [[nodiscard]]
    size_t capacity() const noexcept { return _buffer.capacity(); }

    [[nodiscard]]
    bool empty() const noexcept { return _buffer.empty(); }

    // get size of block
    [[nodiscard]]
    size_t blockSize() const noexcept { return _blockSize; }

    // get number of blocks
    [[nodiscard]]
    size_t numOfBlocks() const noexcept { return size() / _blockSize; }

    // get block by index
    [[nodiscard]]
    Pointer get(size_t idx) {
        assert(idx * _blockSize < _buffer.size());
        return _buffer.data() + idx * _blockSize;
    }

    // get block by index
    [[nodiscard]]
    ConstPointer get(size_t idx) const {
        assert(idx * _blockSize < _buffer.size());
        return _buffer.data() + idx * _blockSize;
    }

    // check if a pointer belongs to this buffer
    [[nodiscard]]
    bool owns(ConstPointer p) const noexcept {
        ConstPointer begin = _buffer.data();
        ConstPointer end   = begin + _buffer.size();
        return p >= begin && p < end;
    }

private:
    std::vector<char> _buffer;
    size_t            _blockSize { 0 };
};

static_assert(std::is_move_constructible_v<BlocksBuffer>);
static_assert(std::is_move_assignable_v<BlocksBuffer>);
static_assert( ! std::is_copy_constructible_v<BlocksBuffer>);
static_assert( ! std::is_copy_assignable_v<BlocksBuffer>);

using FileLineRef  = std::string_view;
using FileLineRefs = std::vector<FileLineRef>;

// Simplified class for effective storage of blocks of file lines
class LinesBlock final {
public:

    LinesBlock() noexcept = default;

    LinesBlock(size_t maxLines, bool withBuffer,
                    size_t bufferBlockSize = BlocksBuffer::DEFAULT_BLOCK_SIZE) {
        alloc(maxLines, withBuffer, bufferBlockSize);
    }

    LinesBlock& operator=(const LinesBlock& other) {

        if(other._buffer.empty()) {
            _lines = other._lines;
            _buffer.clear();
            return *this;
        }

        if(_buffer.size() != other._buffer.size()) {

            if(_buffer.capacity() >= other._buffer.size()) {
                // there is no real memory allocation here
                _buffer.resize(other._buffer.numOfBlocks(), other._buffer.blockSize());
                assert(_buffer.blockSize() == other._buffer.blockSize());
                assert(_buffer.size() == other._buffer.size());
            }
            else {
                throw std::logic_error(
                    "LinesBlock does not support copying with memory reallocation");
            }
        }

        _lines.clear();
        for(size_t i = 0; i < other._lines.size(); ++i) {
            auto& othrline = other._lines[i];
            auto* buff     = _buffer.get(i);

            assert(othrline.cbegin() >= other._buffer.get(i));
            assert(othrline.cend() < other._buffer.get(i) + other._buffer.blockSize());

            std::memcpy(buff, othrline.cbegin(), othrline.size());
            _lines.emplace_back(buff, othrline.size());
        }

        return *this;
    }

    LinesBlock(LinesBlock&&) noexcept = default;
    LinesBlock& operator=(LinesBlock&& other) noexcept = default;

    void swap(LinesBlock& other) noexcept {
        _buffer.swap(other._buffer);
        _lines.swap(other._lines);
    }

    void alloc(size_t maxLines, bool withBuffer,
                    size_t bufferBlockSize = BlocksBuffer::DEFAULT_BLOCK_SIZE) {
        _lines.reserve(maxLines);
        if(withBuffer) {
            _buffer.resize(maxLines, bufferBlockSize);
        }
    }

    void addLine(const FileLineRef& line) {
        assert(checkLine(line));
        _lines.push_back(line);
    }

    void addLine(FileLineRef&& line) {
        assert(checkLine(line));
        _lines.push_back(std::move(line));
    }

    // clear lines, it does not deallocate memory
    void clear() noexcept {
        _lines.clear();
    }

    [[nodiscard]]
    const FileLineRefs& lines() const noexcept { return _lines; }

    [[nodiscard]]
    const BlocksBuffer& buffer() const noexcept { return _buffer; }

    [[nodiscard]]
    BlocksBuffer& buffer() noexcept { return _buffer; }

private:
    BlocksBuffer _buffer;
    FileLineRefs _lines;

    [[nodiscard]]
    bool checkLine(const FileLineRef& line) const noexcept {
        if(_buffer.empty() || line.empty()) {
            return true;
        }

        auto* begin = line.data();
        auto* last = begin + line.size() - 1;
        return _buffer.owns(begin) && _buffer.owns(last);
    }
};

static_assert(std::is_move_constructible_v<LinesBlock>);
static_assert(std::is_move_assignable_v<LinesBlock>);
static_assert( ! std::is_copy_constructible_v<LinesBlock>);
static_assert(std::is_copy_assignable_v<LinesBlock>);

using LinesBlockPtr = LinesBlock*;

// This is something that is similar to local allocator
class LinesBlockPool final {
public:

    LinesBlockPool(size_t numOfBlocks, size_t maxLines,
                            size_t blockSize = BlocksBuffer::DEFAULT_BLOCK_SIZE):
        _freeBlocks(numOfBlocks), _maxLines(maxLines), _blockSize(blockSize) {

        assert(numOfBlocks > 0);
        assert(maxLines > 0);
        assert(blockSize > 0);

        _blocks.reserve(numOfBlocks);
        for(size_t i = 0; i < numOfBlocks; ++i) {
            _blocks.emplace_back(maxLines, false);
        }
    }

    // init/reset blocks
    void reset(bool allocBuffers) {

        // reset all pointers of free blocks
        _freeBlocks.reset();
        for(auto& block: _blocks) {
            _freeBlocks.push(&block);
            block.alloc(_maxLines, allocBuffers, _blockSize);
        }
    }

    // allocate block, there is no memory allocation
    [[nodiscard]]
    LinesBlockPtr allocBlock() {
        if(_freeBlocks.empty()) {
            return nullptr;
        }

        auto p = _freeBlocks.top();
        _freeBlocks.pop();
        return p;
    }

    // free block, there is no memory deallocation
    void freeBlock(LinesBlockPtr p) {
        _freeBlocks.push(p);
    }

    [[nodiscard]]
    size_t capacity() const noexcept { return _blocks.capacity(); }

private:
    using VectorOfBlocks = std::vector<LinesBlock>;
    using BlockPtrsRing  = SimpleRingBuffer<LinesBlockPtr>;

    VectorOfBlocks _blocks;
    BlockPtrsRing  _freeBlocks;
    const size_t   _maxLines;
    const size_t   _blockSize;
};
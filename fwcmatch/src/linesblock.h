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

    //constexpr static size_t DEFAULT_BLOCK_SIZE = 4*1024;
    constexpr static size_t DEFAULT_BLOCK_SIZE = 2*1024;

    explicit BlocksBuffer(size_t size = 0, size_t blockSize = DEFAULT_BLOCK_SIZE):
        _blockSize(blockSize) {

        if(size) {
            resize(size, blockSize);
        }
    }

    BlocksBuffer(BlocksBuffer&& other) noexcept = default;
    BlocksBuffer& operator=(BlocksBuffer&& other) noexcept = default;

    // resize buffer
    // there is no real memory allocation if capacity() returns enough big size
    void resize(size_t numOfBlocks, size_t blockSize = DEFAULT_BLOCK_SIZE) {
        assert(blockSize > 0);
        _blockSize = blockSize;
        _buffer.resize(_blockSize * numOfBlocks);
    }

    void swap(BlocksBuffer& other) noexcept {
        _buffer.swap(other._buffer);
        std::swap(_blockSize, other._blockSize);
    }

    void clear() noexcept { _buffer.clear(); }

    // get current buffer size
    size_t size() const noexcept { return _buffer.size(); }

    // get capacity of buffer
    size_t capacity() const noexcept { return _buffer.capacity(); }

    bool empty() const noexcept { return _buffer.empty(); }

    // get size of block
    size_t blockSize() const noexcept { return _blockSize; }

    // get number of blocks
    size_t numOfBlocks() const noexcept { return size() / _blockSize; }

    // get block by index
    Pointer get(size_t idx) {
        assert(idx * _blockSize < _buffer.size());
        return _buffer.data() + idx * _blockSize;
    }

    // get block by index
    ConstPointer get(size_t idx) const {
        assert(idx * _blockSize < _buffer.size());
        return _buffer.data() + idx * _blockSize;
    }

private:
    std::vector<char> _buffer;
    size_t            _blockSize;
};

static_assert(std::is_move_constructible_v<BlocksBuffer>);
static_assert(std::is_move_assignable_v<BlocksBuffer>);
static_assert( ! std::is_copy_constructible_v<BlocksBuffer>);
static_assert( ! std::is_copy_assignable_v<BlocksBuffer>);

using FileLineRef  = std::string_view;
using FileLineRefs = std::vector<FileLineRef>;

// Simplified structure for effective storage of blocks of file lines
struct LinesBlock {
    BlocksBuffer buffer;
    FileLineRefs lines;

    LinesBlock() = default;

    LinesBlock& operator=(const LinesBlock& other) {

        if(other.buffer.empty()) {
            lines = other.lines;
            buffer.clear();
            return *this;
        }

        if(buffer.size() != other.buffer.size()) {

            if(buffer.capacity() >= other.buffer.size()) {
                // there is no real memory allocation here
                buffer.resize(other.buffer.numOfBlocks(), other.buffer.blockSize());
                assert(buffer.blockSize() == other.buffer.blockSize());
                assert(buffer.size() == other.buffer.size());
            }
            else {
                throw std::logic_error(
                    "LinesBlock does not support copying with memory reallocation");
            }
        }

        lines.clear();
        for(size_t i = 0; i < other.lines.size(); ++i) {
            auto& othrline = other.lines[i];
            auto* buff     = buffer.get(i);

            assert(othrline.cbegin() >= other.buffer.get(i));
            assert(othrline.cend() < other.buffer.get(i) + other.buffer.blockSize());

            std::memcpy(buff, othrline.cbegin(), othrline.size());
            lines.emplace_back( buff, othrline.size() );
        }

        return *this;
    }

    void swap(LinesBlock& other) noexcept {
        buffer.swap(other.buffer);
        lines.swap(other.lines);
    }

    LinesBlock(LinesBlock&&) noexcept = default;
    LinesBlock& operator=(LinesBlock&& other) noexcept = default;
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
            LinesBlock blk;
            blk.lines.reserve(maxLines);
            _blocks.push_back(std::move(blk));
        }
    }

    // init/reset blocks
    void reset(bool allocBuffers) {

        // reset all pointers of free blocks
        _freeBlocks.reset();
        for(auto& block: _blocks) {
            _freeBlocks.push(&block);
            if(allocBuffers) {
                block.buffer.resize(_maxLines, _blockSize);
            }
        }
    }

    // allocate block, there is no memory allocation
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

    // get size of block
    size_t blockSize() const noexcept { return _blockSize; }

    // get max number of lines
    size_t maxLines() const noexcept { return _maxLines; }

    size_t capacity() const noexcept { return _blocks.capacity(); }

private:
    using VectorOfBlocks = std::vector<LinesBlock>;
    using BlockPtrsRing  = SimpleRingBuffer<LinesBlockPtr>;

    VectorOfBlocks _blocks;
    BlockPtrsRing  _freeBlocks;
    const size_t   _maxLines;
    const size_t   _blockSize;
};
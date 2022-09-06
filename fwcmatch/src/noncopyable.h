#pragma once

// I don't like to write deleted ctor/assign inside each a class but don't want
// to add boost as a depedency to this project

namespace fwc {

// Class to disable copying
class noncopyable {
protected:
    constexpr noncopyable() = default;
    ~noncopyable() = default;

    noncopyable(noncopyable const&) = delete;
    noncopyable& operator=(noncopyable const&) = delete;
};

} // namespace fwc
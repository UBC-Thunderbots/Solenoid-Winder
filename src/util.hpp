#pragma once
namespace util {

// these are prefixed by n because F### macros
template <typename T>
T nmin(T a, T b) {
    return (a < b) ? a : b;
}

template <typename T>
T nmax(T a, T b) {
    return (a > b) ? a : b;
}

}
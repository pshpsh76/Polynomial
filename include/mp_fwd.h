#pragma once

#include <cstddef>

#include "complex_type.h"

template <typename T, typename U>
using AddType = decltype(std::declval<T>() + std::declval<U>());

template <typename T, typename U>
using SubType = decltype(std::declval<T>() - std::declval<U>());

template <typename T, typename U>
using MultiplyType = decltype(std::declval<T>() * std::declval<U>());

struct DefaultPow {
    template <typename T>
    T operator()(const T& object, size_t pow) {
        T res(GetOne(object));
        for (size_t i = 0; i < pow; ++i) {
            res *= object;
        }
        return res;
    }
};

struct BinaryPow {
    template <typename T>
    T operator()(const T& object, size_t pow) {
        if (pow == 0) {
            return GetOne(object);
        }
        if (pow % 2 == 1) {
            return object * (*this)(object, pow - 1);
        }
        T tmp = (*this)(object, pow / 2);
        return tmp * tmp;
    }
};

namespace SingleVariable {

template <typename T, typename PowFunction = DefaultPow>
class Monomial;

template <typename T, typename PowFunction = DefaultPow>
class Polynomial;

template <typename T>
struct is_polynomial : std::false_type {};

template <typename T, typename PowFunction>
struct is_polynomial<Polynomial<T, PowFunction>> : std::true_type {};

template <typename T>
concept NotPolynomial = !is_polynomial<T>::value;

}  // namespace SingleVariable

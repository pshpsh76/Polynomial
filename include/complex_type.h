#pragma once

#include <any>
#include <type_traits>

struct DefaultParams {
    std::any data;
};

class ComplexType {
public:
    virtual struct DefaultParams GetDefaultParams() const = 0;
};

template <typename T>
T GetConst(const T& value, int cnst) {
    if constexpr (std::is_base_of_v<ComplexType, T>) {
        return T(value.GetDefaultParams(), cnst);
    } else {
        return T(cnst);
    }
}

template <typename T>
T GetZero(const T& value) {
    return GetConst(value, 0);
}

template <typename T>
T GetOne(const T& value) {
    return GetConst(value, 1);
}

#pragma once

#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "complex_type.h"

template <typename T>
class Matrix;

template <typename T>
Matrix<T> Identity(size_t n);

template <class T>
class Matrix : public ComplexType {
public:
    Matrix(size_t rows, size_t cols) : data_(rows, std::vector<T>(cols)) {
    }

    Matrix(size_t dim) : data_(dim, std::vector<T>(dim)) {
    }

    Matrix(const std::vector<std::vector<T>>& data) : data_(data) {
    }

    Matrix(const std::initializer_list<std::vector<T>>& data) : data_(data) {
    }

    Matrix(const struct DefaultParams& params, const T& value) {
        T one = GetOne(value);
        auto shape = std::any_cast<std::pair<size_t, size_t>>(params.data);
        if (shape.first != shape.second || value != one) {
            data_.resize(shape.first, std::vector<T>(shape.second, value));
        } else {
            *this = std::move(Identity<T>(shape.first));
        }
    }

    std::pair<size_t, size_t> Shape() const {
        if (data_.empty()) {
            return {0, 0};
        }
        return {data_.size(), data_[0].size()};
    }

    size_t Rows() const {
        return data_.size();
    }

    size_t Columns() const {
        if (data_.empty()) {
            return 0;
        }
        return data_[0].size();
    }

    T& operator()(size_t i, size_t j) {
        return data_[i][j];
    }

    const T& operator()(size_t i, size_t j) const {
        return data_[i][j];
    }

    Matrix& operator+=(const Matrix& rhs) {
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                data_[i][j] += rhs.data_[i][j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& rhs) {
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                data_[i][j] -= rhs.data_[i][j];
            }
        }
        return *this;
    }

    Matrix& operator*=(const T& multiplyer) {
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                data_[i][j] *= multiplyer;
            }
        }
        return *this;
    }

    Matrix& operator*=(const Matrix& rhs) {
        *this = *this * rhs;
        return *this;
    }

    struct DefaultParams GetDefaultParams() const override {
        return DefaultParams{Shape()};
    }

private:
    std::vector<std::vector<T>> data_;
};

template <typename T>
Matrix<T> Transpose(const Matrix<T>& matrix) {
    Matrix<T> transposed(matrix.Columns(), matrix.Rows());
    for (size_t i = 0; i < transposed.Rows(); ++i) {
        for (size_t j = 0; j < transposed.Columns(); ++j) {
            transposed(i, j) = matrix(j, i);
        }
    }
    return transposed;
}

template <typename T>
Matrix<T> Identity(size_t n) {
    T one = GetOne(T{});
    Matrix<T> e(n);
    for (size_t i = 0; i < n; ++i) {
        e(i, i) = one;
    }
    return e;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs) {
    if (lhs.Columns() != rhs.Rows()) {
        throw std::runtime_error{"Not valid dims for multiply matrix"};
    }
    Matrix<T> rhs_t(Transpose(rhs));
    Matrix<T> res(lhs.Rows(), rhs.Columns());
    for (size_t i = 0; i < res.Rows(); ++i) {
        for (size_t j = 0; j < res.Columns(); ++j) {
            for (size_t k = 0; k < lhs.Columns(); ++k) {
                res(i, j) += lhs(i, k) * rhs_t(j, k);
            }
        }
    }
    return res;
}

template <typename T>
Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs) {
    Matrix<T> tmp = lhs;
    tmp += rhs;
    return tmp;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs) {
    Matrix<T> tmp = lhs;
    tmp -= rhs;
    return tmp;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& lhs, const T& multiplyer) {
    Matrix<T> tmp = lhs;
    tmp *= multiplyer;
    return tmp;
}

template <typename T, typename U>
Matrix<T> operator*(const U& multiplyer, const Matrix<T>& rhs) {
    Matrix<T> tmp(rhs.Rows(), rhs.Columns());
    for (size_t i = 0; i < rhs.Rows(); ++i) {
        for (size_t j = 0; j < rhs.Columns(); ++j) {
            tmp(i, j) = multiplyer * rhs(i, j);
        }
    }
    return tmp;
}

template <typename T>
bool operator==(const Matrix<T>& a, const Matrix<T>& b) {
    if (a.Rows() != b.Rows()) {
        return false;
    }
    if (a.Columns() != b.Columns()) {
        return false;
    }
    for (size_t i = 0; i < a.Rows(); ++i) {
        for (size_t j = 0; j < a.Columns(); ++j) {
            if (a(i, j) != b(i, j)) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& matrix) {
    for (size_t row = 0; row < matrix.Rows(); ++row) {
        for (size_t col = 0; col < matrix.Columns(); ++col) {
            os << matrix(row, col) << ' ';
        }
        os << '\n';
    }
    return os;
}

#include <catch.hpp>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../include/matrix.h"

template <typename T>
bool EqualMatrix(const Matrix<T>& a, const Matrix<T>& b) {
    if (a.Rows() != b.Rows()) {
        return false;
    }
    if (a.Columns() != b.Columns()) {
        return false;
    }
    for (size_t i = 0; i < a.Rows(); ++i) {
        for (size_t j = 0; j < a.Columns(); ++j) {
            if (a(i, j) != Approx(b(i, j))) {
                return false;
            }
        }
    }
    return true;
}

TEST_CASE("Constructors") {
    {
        Matrix<int> a(3);
        REQUIRE(3u == a.Rows());
        REQUIRE(3u == a.Columns());
    }
    {
        Matrix<int> a(3, 5);
        REQUIRE(3u == a.Rows());
        REQUIRE(5u == a.Columns());
    }
    {
        Matrix<int> a({{1, 2}, {3, 4}, {5, 6}});
        REQUIRE(3u == a.Rows());
        REQUIRE(2u == a.Columns());
        REQUIRE(3 == a(1, 0));
    }
}

TEST_CASE("Constness") {
    {
        Matrix<int> a({{1, 2}, {3, 4}});
        const Matrix<int>& b = a;
        REQUIRE(2u == b.Rows());
        REQUIRE(2u == b.Columns());
        REQUIRE(2 == b(0, 1));
    }
    {
        const Matrix<int> first = Identity<int>(3);
        const Matrix<int> second(3);
        REQUIRE(true == EqualMatrix(first, Transpose(first)));
        REQUIRE(true == EqualMatrix(second, first - first));
        REQUIRE(true == EqualMatrix(first, first * first));
    }
}

TEST_CASE("Identity and Zero from constructor") {
    Matrix<int> a(3, 3);
    Matrix<int> b(a.GetDefaultParams(), 1);
    REQUIRE(EqualMatrix(b, Identity<int>(3)));
    Matrix<int> c(a.GetDefaultParams(), 0);
    REQUIRE(EqualMatrix(c, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}));
}

TEST_CASE("Operations") {
    Matrix<double> a({{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    Matrix<double> b({{0.0, 1.0, 0.0}, {1.0, 1.0, 2.0}});
    Matrix<double> c({{-1.0, -1.0}, {1.0, 1.0}, {1.0, -1.0}});

    REQUIRE(false == EqualMatrix(a, Transpose(a)));
    REQUIRE(true ==
            EqualMatrix(Transpose(a), Matrix<double>({{1.0, 4.0}, {2.0, 5.0}, {3.0, 6.0}})));

    Matrix<double> old_a = a;
    REQUIRE(true == EqualMatrix(a += b, Matrix<double>({{1.0, 3.0, 3.0}, {5.0, 6.0, 8.0}})));
    REQUIRE(true == EqualMatrix(a -= b, old_a));
    REQUIRE(true == EqualMatrix(a -= a, Matrix<double>(2, 3)));

    REQUIRE(true == EqualMatrix(b * c, Matrix<double>({{1.0, 1.0}, {2.0, -2.0}})));
}

TEST_CASE("Basic ops") {
    Matrix<int> left(2, 3);
    Matrix<int> right(2, 3);
    Matrix<int> sub(2, 3);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            left(i, j) = i;
            right(i, j) = j;
            sub(i, j) = i == j;
        }
    }
    Matrix<int> test = left + right - sub;
    REQUIRE(test.Rows() == 2);
    REQUIRE(test.Columns() == 3);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            REQUIRE(test(i, j) == i + j - (i == j));
        }
    }
}

TEST_CASE("Multipling") {
    Matrix<int> left{{1, 2, 3}, {4, 5, 6}};
    Matrix<int> right{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};
    Matrix<int> test = left * right;
    std::vector<std::vector<int>> expected{{38, 44, 50, 56}, {83, 98, 113, 128}};

    REQUIRE(test.Rows() == 2);
    REQUIRE(test.Columns() == 4);
    for (size_t i = 0; i < test.Rows(); ++i) {
        for (size_t j = 0; j < test.Columns(); ++j) {
            REQUIRE(test(i, j) == expected[i][j]);
        }
    }
}

TEST_CASE("Mixed ops") {
    Matrix<uint64_t> left{{1, 1}, {1, 1}};
    const auto inf = std::numeric_limits<uint64_t>::max();
    Matrix<uint64_t> right{{inf, inf}, {inf, inf}};
    Matrix<uint64_t> mul{{1, 2}, {3, 4}};

    Matrix<uint64_t> result = mul * (left + right);
    REQUIRE(result.Rows() == 2);
    REQUIRE(result.Columns() == 2);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            REQUIRE(result(i, j) == 0);
        }
    }
}

TEST_CASE("Exceptions") {
    Matrix<int> first(2, 3);
    Matrix<int> second(3, 4);
    Matrix<int> third(4, 6);
    Matrix<int> fourth(5, 7);
    REQUIRE_THROWS_AS(Matrix<int>(first * second * third * fourth), std::runtime_error);
}

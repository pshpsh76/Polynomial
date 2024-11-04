#include <catch.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <set>

#include "../include/matrix.h"
#include "../include/monomial.h"
#include "../include/polynomial.h"
#include "structs.h"

TEST_CASE("Parsing") {
    auto x = SingleVariable::ParseFromString<int>("x");
    REQUIRE(x == SingleVariable::Polynomial<int>{{1, 1}});
    x = SingleVariable::ParseFromString<int>("-2x");
    REQUIRE(x == SingleVariable::Polynomial<int>{{-2, 1}});
    x = SingleVariable::ParseFromString<int>("x^37291");
    REQUIRE(x == SingleVariable::Polynomial<int>{{1, 37291}});
    x = SingleVariable::ParseFromString<int>("23721817x^3211234");
    REQUIRE(x == SingleVariable::Polynomial<int>{{23721817, 3211234}});

    auto p = SingleVariable::ParseFromString<int>("x + 23721817x^3211234 - 2x + -x^37291 + 37891");
    REQUIRE(p ==
            SingleVariable::Polynomial<int>{{23721817, 3211234}, {-1, 37291}, {-1, 1}, {37891, 0}});
}

TEST_CASE("Const polynoms") {
    {
        SingleVariable::Polynomial<int> a(5);
        auto expected_1 =
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{5, 0}};
        REQUIRE(a.GetMonomials() == expected_1);
        SingleVariable::Polynomial<double> b(3.5);
        auto expected_2 =
            std::set<SingleVariable::Monomial<double>>{SingleVariable::Monomial<double>{3.5, 0}};
        REQUIRE(b.GetMonomials() == expected_2);
    }
    {
        for (int i = 0; i < 1000; ++i) {
            int num = rand();
            SingleVariable::Polynomial<int> a(num);
            auto expected =
                std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{num, 0}};
            REQUIRE(a.GetMonomials() == expected);
        }
    }
    {
        SingleVariable::Polynomial<Int> a(Int{493});
        auto expected =
            std::set<SingleVariable::Monomial<Int>>{SingleVariable::Monomial<Int>{Int{493}, 0}};
        REQUIRE(a.GetMonomials() == expected);
    }
    {
        SingleVariable::Polynomial<Matrix<int>> a(Matrix<int>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
        auto expected = std::set<SingleVariable::Monomial<Matrix<int>>>{
            SingleVariable::Monomial<Matrix<int>>{Matrix<int>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, 0}};
        REQUIRE(a.GetMonomials() == expected);
    }
}

TEST_CASE("Add const") {
    SingleVariable::Polynomial<int> poly;
    poly += 10;
    REQUIRE(poly.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{10, 0}});

    poly += 4;
    REQUIRE(poly.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{14, 0}});

    auto other = poly + 463;
    REQUIRE(poly.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{14, 0}});
    REQUIRE(other.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{477, 0}});

    auto other_2 = 23 + other;
    REQUIRE(poly.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{14, 0}});
    REQUIRE(other_2.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{500, 0}});

    SingleVariable::Polynomial<Matrix<int>> matrix_poly;
    matrix_poly += Matrix<int>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    matrix_poly += Identity<int>(3);
    REQUIRE(matrix_poly.GetMonomials() == std::set<SingleVariable::Monomial<Matrix<int>>>{
                                              Matrix<int>{{2, 2, 3}, {4, 6, 6}, {7, 8, 10}}});
}

TEST_CASE("Add polynom") {
    SingleVariable::Polynomial<int> a;
    SingleVariable::Polynomial<int> b;
    a += b;
    REQUIRE(a.GetMonomials() == std::set<SingleVariable::Monomial<int>>{});
    SingleVariable::Polynomial<int> c{{1, 2}, {3, 4}};  // 3x^4 + x^2
    b += c;
    REQUIRE(b == c);
    REQUIRE(c.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{3, 4},
                                                    SingleVariable::Monomial<int>{1, 2}});

    b += a;
    REQUIRE(b == c);

    auto d = c + b;
    REQUIRE(d == (2 * b));
    REQUIRE(b == c);

    SingleVariable::Polynomial<int> e = {{4, 3}, {2, 1}};
    d += e;
    auto expected = (2 * b).GetMonomials();
    expected.insert(SingleVariable::Monomial<int>{4, 3});
    expected.insert(SingleVariable::Monomial<int>{2, 1});
    REQUIRE(d.GetMonomials() == expected);
}

TEST_CASE("Zero polynom") {
    auto a = SingleVariable::GetZeroPolynomial<int>();
    REQUIRE(a.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{0, 0}});

    SingleVariable::Polynomial<int> b{{1, 2}, {3, 4}};
    b += a;
    REQUIRE(b.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{3, 4},
                                                    SingleVariable::Monomial<int>{1, 2},
                                                    SingleVariable::Monomial<int>{0, 0}});
    a = a * 10;
    REQUIRE(a == SingleVariable::GetZeroPolynomial<int>());

    auto matrix = Identity<int>(5);
    auto matrix_zero = SingleVariable::GetZeroPolynomialSafe(matrix);
    REQUIRE(matrix_zero.GetMonomials() ==
            std::set<SingleVariable::Monomial<Matrix<int>>>{Matrix<int>{5}});
}

TEST_CASE("Unary minus") {
    SingleVariable::Polynomial<int> a{{1, 2}, {3, 4}};
    auto b = -a;
    REQUIRE(b.GetMonomials() ==
            std::set<SingleVariable::Monomial<int>>{SingleVariable::Monomial<int>{-3, 4},
                                                    SingleVariable::Monomial<int>{-1, 2}});
    auto c = -SingleVariable::GetZeroPolynomial<int>();
    REQUIRE(c == SingleVariable::GetZeroPolynomial<int>());
}

TEST_CASE("Multiply const") {
    SingleVariable::Polynomial<int> a{{1, 2}, {3, 4}, {5, 6}};
    a *= 2;
    {
        auto expected = std::set<SingleVariable::Monomial<int>>{
            SingleVariable::Monomial<int>{2, 2}, SingleVariable::Monomial<int>{6, 4},
            SingleVariable::Monomial<int>{10, 6}};
        REQUIRE(a.GetMonomials() == expected);
    }
    {
        auto b = 5 * a;
        auto expected = std::set<SingleVariable::Monomial<int>>{
            SingleVariable::Monomial<int>{10, 2}, SingleVariable::Monomial<int>{30, 4},
            SingleVariable::Monomial<int>{50, 6}};
        REQUIRE(b.GetMonomials() == expected);
        auto c = 0.2 * b;
        auto expected_2 = std::set<SingleVariable::Monomial<double>>{
            SingleVariable::Monomial<double>{2, 2}, SingleVariable::Monomial<double>{6, 4},
            SingleVariable::Monomial<double>{10, 6}};
        REQUIRE(c.GetMonomials() == expected_2);
    }
    {
        auto b = 0 * a;
        auto expected = std::set<SingleVariable::Monomial<int>>{
            SingleVariable::Monomial<int>{0, 2}, SingleVariable::Monomial<int>{0, 4},
            SingleVariable::Monomial<int>{0, 6}};
        REQUIRE(b.GetMonomials() == expected);
    }
}

TEST_CASE("Multiply poly") {
    auto zero = SingleVariable::GetZeroPolynomial<int>();
    SingleVariable::Polynomial<int> p = {{1, 1}, {2, 0}};  // x + 2
    SingleVariable::Polynomial<int> p2 = {{1, 2}, {4, 1}, {4, 0}};
    SingleVariable::Polynomial<int> p3 = {{1, 3}, {6, 2}, {12, 1}, {8, 0}};
    SingleVariable::Polynomial<double> a{{1.5, 5}, {-2, 3}};
    SingleVariable::Polynomial<double> b{{1.5, 6}, {3, 5}, {-2, 4}, {-4, 3}};

    REQUIRE(p * p2 == p3);
    REQUIRE(p3 * SingleVariable::GetOnePolynomial<int>() == p3);
    p3 *= zero;
    p3.Reduce();
    zero.Reduce();
    REQUIRE(p3 == zero);

    REQUIRE(a * p == b);
}

TEST_CASE("Pow") {
    SingleVariable::Polynomial<int> p = {{1, 1}, {2, 0}};  // x + 2
    SingleVariable::Polynomial<int> p2 = {{1, 2}, {4, 1}, {4, 0}};
    SingleVariable::Polynomial<int> p3 = {{1, 3}, {6, 2}, {12, 1}, {8, 0}};
    REQUIRE(DefaultPow()(p, 0) == SingleVariable::GetOnePolynomial<int>());
    REQUIRE(DefaultPow()(p, 1) == p);
    REQUIRE(DefaultPow()(p, 2) == p2);
    REQUIRE(DefaultPow()(p, 3) == p3);
}

TEST_CASE("Substitution polynom") {
    SingleVariable::Polynomial<int> p = {{1, 1}, {5, 2}, {3, 3}};
    SingleVariable::Polynomial<int> x = {{1, 1}};
    REQUIRE(p(x) == p + SingleVariable::GetZeroPolynomial<int>());
    REQUIRE(p(x * x) == x * x + 5 * DefaultPow()(x, 4) + 3 * DefaultPow()(x, 6) +
                            SingleVariable::GetZeroPolynomial<int>());
    REQUIRE(x(p) == p + SingleVariable::GetZeroPolynomial<int>());
    REQUIRE((x + 2)(x + 1) == x + 3);
}

TEST_CASE("Substitution point") {
    SingleVariable::Polynomial<size_t> p = {{6483234, 4733}, {4373, 393}, {-372721, 7437}};
    REQUIRE(p(1) == 6114886);
    REQUIRE(DefaultPow()(p, 4)(0) == 0);

    SingleVariable::Polynomial<int> p2 = {{1, 1}, {1, 0}};
    auto p3 = DefaultPow()(p2, 10);
    REQUIRE(p3(1) == 1024);
    REQUIRE(p2(0.5) == 1.5);
}

TEST_CASE("Substitution matrix") {
    Matrix<int> m = {{1, 0, 2}, {3, 5, 0}, {1, 1, 8}};
    SingleVariable::Polynomial<int> xi = {{-1, 3}, {14, 2}, {-51, 1}, {36, 0}};
    REQUIRE(xi(m) == GetZero(m));
}

TEST_CASE("Reduce") {
    SingleVariable::Polynomial<int> a = {{-1, 3}, {14, 2}, {-51, 1}, {36, 0}};
    a -= a;
    a.Reduce();
    REQUIRE(a.GetMonomials() == std::set<SingleVariable::Monomial<int>>{});
}
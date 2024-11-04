#include <catch.hpp>
#include <cstdlib>
#include <set>

#include "../include/matrix.h"
#include "../include/monomial.h"
#include "../include/polynomial.h"

TEST_CASE("Constructors and basic ops") {
    SingleVariable::Monomial<int> a;
    REQUIRE(a.GetCoef() == 0);
    REQUIRE(a.GetDegree() == 0);
    REQUIRE(a(rand()) == 0);

    SingleVariable::Monomial<double> b{1, 2};
    b.GetCoef() /= 2;
    SingleVariable::Monomial<int> c{7, 1};
    b *= c;
    REQUIRE(b.GetCoef() == 3.5);
    REQUIRE(b.GetDegree() == 3);

    b *= 2;
    auto d = b * 5;
    REQUIRE(d.GetCoef() == 35.0);
    REQUIRE(d.GetDegree() == 3);
}

TEST_CASE("Substitution") {
    SingleVariable::Monomial<int> a{2, 4};  // 2x^4
    REQUIRE(a(3) == 162);
    REQUIRE(a(1.5) == 10.125);

    SingleVariable::Monomial<double> b{5.5, 2};  // 5.5x^2
    REQUIRE(b(a) == SingleVariable::Monomial<double>{22, 8});
    REQUIRE(a(b) == SingleVariable::Monomial<double>{1830.125, 8});
    {
        auto matrix = Identity<int>(3);
        auto res = 2 * matrix;
        REQUIRE(a(matrix) == res);
    }
    {
        auto matrix = Matrix<int>{{2, 1, 5}, {3, 3, 7}, {8, 0, 4}};
        auto res = Matrix<int>{{8680, 1182, 8262}, {14522, 2022, 13958}, {11024, 1568, 10848}};
        REQUIRE(a(matrix) == res);
    }
    {
        SingleVariable::Polynomial<int> poly{{10, 0}, {5, 3}, {2, 1}};  // 5x^3 + 2x + 10
        auto expected = std::set<SingleVariable::Monomial<double>>{
            SingleVariable::Monomial<double>{137.5, 6}, SingleVariable::Monomial<double>{110, 4},
            SingleVariable::Monomial<double>{550, 3},   SingleVariable::Monomial<double>{22, 2},
            SingleVariable::Monomial<double>{220, 1},   SingleVariable::Monomial<double>{550, 0}};
        REQUIRE(b(poly).GetMonomials() == expected);
    }
}
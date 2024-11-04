#pragma once

#include <cstddef>
#include <ostream>
#include <utility>

#include "mp_fwd.h"  // Forward declaration

namespace SingleVariable {

template <typename T, typename PowFunction>
class Monomial {
public:
    Monomial() = default;

    Monomial(const T& coef, size_t degree) : coef_(coef), degree_(degree) {
    }

    Monomial(const T& coef) : coef_(coef) {
    }

    template <typename U>
    using SubstitutionType =
        MultiplyType<T, decltype(PowFunction()(std::declval<U>(), std::declval<size_t>()))>;

    template <typename U>
    SubstitutionType<U> operator()(const U& point) {
        return coef_ * PowFunction()(point, degree_);
    }

    const size_t& GetDegree() const {
        return degree_;
    }

    size_t& GetDegree() {
        return degree_;
    }

    const T& GetCoef() const {
        return coef_;
    }

    T& GetCoef() {
        return coef_;
    }

    template <typename U>
    Monomial& operator*=(const Monomial<U, PowFunction>& rhs) {
        coef_ *= rhs.GetCoef();
        degree_ += rhs.GetDegree();
        return *this;
    }

    template <typename U>
    Monomial& operator*=(const U& rhs) {
        coef_ *= rhs;
        return *this;
    }

private:
    T coef_ = 0;
    size_t degree_ = 0;
};

template <typename T, typename U, typename PowFunction>
Monomial<MultiplyType<T, U>, PowFunction> operator*(const Monomial<T, PowFunction>& lhs,
                                                    const Monomial<U, PowFunction>& rhs) {
    Monomial<MultiplyType<T, U>, PowFunction> tmp = lhs;
    tmp *= rhs;
    return tmp;
}

template <typename T, typename U, typename PowFunction>
Monomial<MultiplyType<T, U>, PowFunction> operator*(const Monomial<T, PowFunction>& lhs,
                                                    const U& rhs) {
    Monomial<MultiplyType<T, U>, PowFunction> tmp = lhs;
    tmp *= rhs;
    return tmp;
}

template <typename T, typename U, typename PowFunction>
Monomial<MultiplyType<U, T>, PowFunction> operator*(const U& lhs,
                                                    const Monomial<T, PowFunction>& rhs) {
    return Monomial<MultiplyType<U, T>, PowFunction>{lhs * rhs.GetCoef(), rhs.GetDegree()};
}

template <typename T, typename PowFunction>
bool operator==(const Monomial<T, PowFunction>& lhs, const Monomial<T, PowFunction>& rhs) {
    return lhs.GetCoef() == rhs.GetCoef() && lhs.GetDegree() == rhs.GetDegree();
}

template <typename T, typename PowFunction>
bool operator<(const Monomial<T, PowFunction>& lhs, const Monomial<T, PowFunction>& rhs) {
    return lhs.GetDegree() > rhs.GetDegree();
}

template <typename T, typename PowFunction>
std::ostream& operator<<(std::ostream& os, const Monomial<T, PowFunction>& monom) {
    os << monom.GetCoef();
    if (monom.GetDegree() != 0) {
        os << "x^" << monom.GetDegree();
    }
    return os;
}

}  // namespace SingleVariable

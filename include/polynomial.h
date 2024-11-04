#pragma once

#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "complex_type.h"
#include "monomial.h"
#include "mp_fwd.h"  // Forward declaration

namespace SingleVariable {
template <typename T, typename PowFunction>
class Polynomial {
public:
    template <typename U, typename OtherPow>
    friend class Polynomial;

    Polynomial() = default;
    explicit Polynomial(const T& other) {
        monoms_.insert({0, Monomial<T>{other, 0}});
    }
    Polynomial(const std::initializer_list<std::pair<T, size_t>>& list) {
        for (const auto& [coef, degree] : list) {
            AddMonom(coef, degree);
        }
    }

    template <NotPolynomial U>
    Polynomial& operator+=(const U& other) {
        AddMonom(other, 0);
        return *this;
    }

    template <NotPolynomial U>
    friend Polynomial<AddType<T, U>, PowFunction> operator+(const Polynomial<T, PowFunction>& lhs,
                                                            const U& other) {
        Polynomial<AddType<T, U>, PowFunction> tmp = lhs;
        tmp += other;
        return tmp;
    }

    template <NotPolynomial U>
    friend Polynomial<AddType<U, T>, PowFunction> operator+(const U& other,
                                                            const Polynomial<T, PowFunction>& rhs) {
        Polynomial<AddType<U, T>, PowFunction> tmp = rhs;
        tmp.AddMonom(other, 0);
        return tmp;
    }

    Polynomial& operator+=(const Monomial<T, PowFunction>& other) {
        AddMonom(other);
        return *this;
    }

    friend Polynomial<T, PowFunction> operator+(const Polynomial<T, PowFunction>& lhs,
                                                const Monomial<T, PowFunction>& other) {
        Polynomial<T, PowFunction> tmp = lhs;
        tmp += other;
        return tmp;
    }

    friend Polynomial<T, PowFunction> operator+(const Monomial<T, PowFunction>& other,
                                                const Polynomial<T, PowFunction>& rhs) {
        Polynomial<T, PowFunction> tmp = rhs;
        tmp.AddMonom(other);
        return tmp;
    }

    template <NotPolynomial U>
    Polynomial& operator-=(const U& other) {
        *this += -other;
        return *this;
    }

    template <NotPolynomial U>
    friend Polynomial<AddType<T, U>, PowFunction> operator-(const Polynomial<T, PowFunction>& lhs,
                                                            const U& other) {
        Polynomial<AddType<T, U>, PowFunction> tmp = lhs;
        tmp -= other;
        return tmp;
    }

    template <typename U>
    Polynomial& operator+=(const Polynomial<U, PowFunction>& other) {
        for (auto [degree, monom] : other.monoms_) {
            AddMonom(monom);
        }
        return *this;
    }

    template <typename U>
    friend Polynomial<AddType<T, U>, PowFunction> operator+(const Polynomial<T, PowFunction>& lhs,
                                                            const Polynomial<U, PowFunction>& rhs) {
        Polynomial<AddType<T, U>, PowFunction> tmp = lhs;
        tmp += rhs;
        return tmp;
    }

    Polynomial operator-() {
        auto tmp = *this;
        for (auto& [degree, monom] : tmp.monoms_) {
            monom.GetCoef() *= -1;
        }
        return tmp;
    }

    Polynomial& operator-=(const Polynomial& other) {
        for (auto [degree, monom] : other.monoms_) {
            AddMonom(-monom.GetCoef(), degree);
        }
        return *this;
    }

    Polynomial operator-(const Polynomial& other) {
        auto tmp = *this;
        tmp -= other;
        return tmp;
    }

    template <typename U>
    friend Polynomial<MultiplyType<T, U>, PowFunction> operator*(
        const Polynomial<T, PowFunction>& lhs, const Polynomial<U, PowFunction>& rhs) {
        Polynomial<MultiplyType<T, U>, PowFunction> res;
        for (const auto& [degree_1, monom_1] : lhs.monoms_) {
            for (const auto& [degree_2, monom_2] : rhs.monoms_) {
                res.AddMonom(monom_1.GetCoef() * monom_2.GetCoef(), degree_1 + degree_2);
            }
        }
        return res;
    }

    template <NotPolynomial U>
    friend Polynomial<MultiplyType<U, T>, PowFunction> operator*(
        const U& multiplyer, const Polynomial<T, PowFunction>& poly) {
        Polynomial<MultiplyType<U, T>, PowFunction> tmp;
        for (const auto& [degree, monom] : poly.monoms_) {
            tmp.AddMonom(multiplyer * monom.GetCoef(), degree);
        }
        return tmp;
    }

    template <typename U>
    Polynomial& operator*=(const U& multiplyer) {
        for (auto& [degree, monom] : monoms_) {
            monom.GetCoef() *= multiplyer;
        }
        return *this;
    }

    template <NotPolynomial U>
    friend Polynomial<MultiplyType<U, T>, PowFunction> operator*(
        const Polynomial<T, PowFunction>& poly, const U& multiplyer) {
        Polynomial<MultiplyType<T, U>, PowFunction> tmp;
        for (const auto& [degree, monom] : poly.monoms_) {
            tmp.AddMonom(monom.GetCoef() * multiplyer, degree);
        }
        return tmp;
    }

    Polynomial& operator*=(const Polynomial& other) {
        Polynomial<T, PowFunction> buff;
        for (const auto& [degree_1, monom_1] : monoms_) {
            for (const auto& [degree_2, monom_2] : other.monoms_) {
                buff.AddMonom(monom_1.GetCoef() * monom_2.GetCoef(), degree_1 + degree_2);
            }
        }
        *this = buff;
        return *this;
    }

    template <typename U>
    auto operator()(const U& point) {
        auto res = GetZero(point);
        for (auto [degree, monom] : monoms_) {
            res += monom(point);
        }
        return res;
    }

    void Reduce() {
        std::vector<typename std::unordered_map<size_t, Monomial<T, PowFunction>>::iterator>
            zero_vals;
        for (auto it = monoms_.begin(); it != monoms_.end(); ++it) {
            if (it->second.GetCoef() == GetZero(it->second.GetCoef())) {
                zero_vals.push_back(it);
            }
        }
        for (const auto& it : zero_vals) {
            monoms_.erase(it);
        }
    }

    std::set<Monomial<T, PowFunction>> GetMonomials() const {
        std::set<Monomial<T, PowFunction>> res_lst;
        for (const auto& [degree, monom] : monoms_) {
            res_lst.insert(monom);
        }
        return res_lst;
    }

    friend bool operator==(const Polynomial<T, PowFunction>& lhs,
                           const Polynomial<T, PowFunction>& rhs) {
        return lhs.monoms_ == rhs.monoms_;
    }

private:
    template <typename U>
    void AddMonom(const U& coef, size_t degree) {
        if (monoms_.contains(degree)) {
            monoms_[degree].GetCoef() += coef;
        } else {
            monoms_[degree] = {coef, degree};
        }
    }

    void AddMonom(const Monomial<T, PowFunction>& monom) {
        if (monoms_.contains(monom.GetDegree())) {
            monoms_[monom.GetDegree()].GetCoef() += monom.GetCoef();
        } else {
            monoms_[monom.GetDegree()] = monom;
        }
    }
    std::unordered_map<size_t, Monomial<T, PowFunction>> monoms_;
};

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const Polynomial<T, U>& poly) {
    auto sorted = poly.GetMonomials();
    size_t ind = 0;
    for (auto monom : sorted) {
        int sign = monom.GetCoef() < 0 ? -1 : 1;
        if (sign < 0) {
            os << '-';
            monom.GetCoef() *= -1;
            if (ind != 0) {
                os << ' ';
            }
            os << monom;
        } else {
            if (ind != 0) {
                os << "+ ";
            }
            os << monom;
        }
        os << ' ';
        ++ind;
    }
    return os;
}

template <typename T, typename PowFunction = DefaultPow>
Polynomial<T, PowFunction> GetZeroPolynomial() {
    return Polynomial<T, PowFunction>{T(0)};
}

template <typename T, typename PowFunction = DefaultPow>
Polynomial<T, PowFunction> GetZeroPolynomialSafe(const T& example) {
    return Polynomial<T, PowFunction>{GetZero(example)};
}

template <typename T, typename PowFunction = DefaultPow>
Polynomial<T, PowFunction> GetOnePolynomial() {
    return Polynomial<T, PowFunction>{T(1)};
}

template <typename T, typename PowFunction = DefaultPow>
Polynomial<T, PowFunction> GetOnePolynomialSafe(const T& example) {
    return Polynomial<T, PowFunction>{GetOne(example)};
}

template <typename T, typename PowFunction = DefaultPow>
Polynomial<T, PowFunction> ParseFromString(std::string_view str) {
    Polynomial<T, PowFunction> result;

    std::string s;
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            s.push_back(c);
        }
    }

    std::regex monom_regex(R"(([+-]?)(\d*\.?\d*)?([a-zA-Z])?(\^(-?\d+))?)");
    auto monom_begin = std::sregex_iterator(s.begin(), s.end(), monom_regex);
    auto monom_end = std::sregex_iterator();

    for (auto it = monom_begin; it != monom_end; ++it) {
        std::smatch match = *it;

        std::string sign_str = match[1];
        std::string coef_str = match[2];
        std::string var_str = match[3];
        std::string exp_str = match[5];

        int sign = 1;
        if (sign_str == "-") {
            sign = -1;
        }

        T coef;
        if (coef_str.empty() && !var_str.empty()) {
            coef = static_cast<T>(1);
        } else if (coef_str.empty() && var_str.empty()) {
            continue;
        } else {
            std::istringstream coef_stream(coef_str);
            coef_stream >> coef;
        }
        coef *= sign;

        size_t degree = 0;
        if (!var_str.empty()) {
            if (!exp_str.empty()) {
                std::istringstream exp_stream(exp_str);
                exp_stream >> degree;
            } else {
                degree = 1;
            }
        }

        result += Monomial<T, PowFunction>(coef, degree);
    }

    return result;
}

}  // namespace SingleVariable

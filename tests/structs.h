#include <cstdint>
#include <ostream>

struct Int {
    int64_t x;
    Int() {
        x = 0;
    }

    Int(int64_t x) : x(x) {
    }

    Int& operator+=(Int rhs) {
        x += rhs.x;
        return *this;
    }

    Int& operator-=(Int rhs) {
        x -= rhs.x;
        return *this;
    }

    Int& operator*=(Int rhs) {
        x *= rhs.x;
        return *this;
    }
};

Int operator*(Int a, Int b) {
    return Int{a.x * b.x};
}

Int operator+(Int a, Int b) {
    return Int{a.x + b.x};
}

Int operator-(Int a, Int b) {
    return Int{a.x - b.x};
}

Int operator-(Int a) {
    return Int{-a.x};
}

bool operator==(const Int& lhs, const Int& rhs) {
    return lhs.x == rhs.x;
}

std::ostream& operator<<(std::ostream& os, const Int& num) {
    os << num.x;
    return os;
}
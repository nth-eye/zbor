#ifndef UTL_MAKIMA_H
#define UTL_MAKIMA_H

#include <algorithm>
#include <optional>
#include <limits>
#include <cmath>

namespace utl {

template<class T, size_t N>
struct makima {

    template<size_t n>
    constexpr makima(
        const T(&x)[n], 
        const T(&y)[n], 
        T l_end_derivative = std::numeric_limits<T>::quiet_NaN(),
        T r_end_derivative = std::numeric_limits<T>::quiet_NaN())
    {
        assign<n>(x, y, l_end_derivative, r_end_derivative);
    }

    constexpr auto size() const
    {
        return len;
    }

    constexpr bool valid() const
    {
        return is_valid;
    }

    template<size_t n>
    constexpr bool assign(
        const T(&x)[n], 
        const T(&y)[n], 
        T l_end_derivative = std::numeric_limits<T>::quiet_NaN(),
        T r_end_derivative = std::numeric_limits<T>::quiet_NaN())
    {
        static_assert(4 <= n && n <= N);

        for (size_t i = 0; i < n - 1; ++i) {
            if (x[i + 1] <= x[i])
                return false;
        }
        std::copy(x, x + n, x_);
        std::copy(y, y + n, y_);
        ctor_n(d_, n, std::numeric_limits<T>::quiet_NaN());
        len = n;

        T m2    = (y[3] - y[2]) / (x[3] - x[2]);
        T m1    = (y[2] - y[1]) / (x[2] - x[1]);
        T m0    = (y[1] - y[0]) / (x[1] - x[0]);
        T mm1   = 2 * m0  - m1;
        T mm2   = 2 * mm1 - m0;
        T w1    = abs(m1  - m0 ) + abs(m1  + m0 ) / 2;
        T w2    = abs(mm1 - mm2) + abs(mm1 + mm2) / 2;

        auto nan_check = [] (T& val) {
            if (std::isnan(val))
                val = 0;
        };

        if (std::isnan(l_end_derivative)) {
            d_[0] = (w1 * mm1 + w2 * m0) / (w1 + w2);
            nan_check(d_[0]);
        } else {
            d_[0] = l_end_derivative;
        }

        w1 = abs(m2 - m1 ) + abs(m2 + m1 ) / 2;
        w2 = abs(m0 - mm1) + abs(m0 + mm1) / 2;
        d_[1] = (w1 * m0 + w2 * m1) / (w1 + w2);

        nan_check(d_[1]);

        for (size_t i = 2; i < n - 2; ++i) {
            T mim2  = (y[i - 1] - y[i - 2]) / (x[i - 1] - x[i - 2]);
            T mim1  = (y[i    ] - y[i - 1]) / (x[i    ] - x[i - 1]);
            T mi    = (y[i + 1] - y[i    ]) / (x[i + 1] - x[i    ]);
            T mip1  = (y[i + 2] - y[i + 1]) / (x[i + 2] - x[i + 1]);
            w1      = abs(mip1 - mi  ) + abs(mip1 + mi  ) / 2;
            w2      = abs(mim1 - mim2) + abs(mim1 + mim2) / 2;
            d_[i]   = (w1 * mim1 + w2 * mi) / (w1 + w2);
            nan_check(d_[i]);
        }

        T mnm4      = (y[n - 3] - y[n - 4]) / (x[n - 3] - x[n - 4]);
        T mnm3      = (y[n - 2] - y[n - 3]) / (x[n - 2] - x[n - 3]);
        T mnm2      = (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]);
        T mnm1      = 2 * mnm2 - mnm3;
        T mn        = 2 * mnm1 - mnm2;
        w1          = abs(mnm1 - mnm2) + abs(mnm1 + mnm2) / 2;
        w2          = abs(mnm3 - mnm4) + abs(mnm3 + mnm4) / 2;
        d_[n - 2]   = (w1 * mnm3 + w2 * mnm2) / (w1 + w2);
        nan_check(d_[n - 2]);

        w1 = abs(mn   - mnm1) + abs(mn   + mnm1) / 2;
        w2 = abs(mnm2 - mnm3) + abs(mnm2 + mnm3) / 2;

        if (std::isnan(r_end_derivative)) {
            d_[n - 1] = (w1 * mnm2 + w2 * mnm1) / (w1 + w2);
            nan_check(d_[n - 1]);
        } else {
            d_[n - 1] = r_end_derivative;
        }
        return is_valid = true;
    }

    constexpr std::optional<T> operator()(T x) const 
    {
        auto back = [this] (const T* buf) { 
            return buf[len - 1];
        };
        if (!valid() || x < x_[0] || x > back(x_))
            return std::nullopt;

        if (x == back(x_))
            return back(y_);

        auto i  = std::upper_bound(x_, x_ + len, x) - x_ - 1;
        T x0    = x_[i];
        T x1    = x_[i + 1];
        T y0    = y_[i];
        T y1    = y_[i + 1];
        T s0    = d_[i];
        T s1    = d_[i + 1];
        T dx    = (x1 - x0);
        T t     = (x - x0) / dx;
        T y     = (1 - t) * (1 - t) * (y0 * (1 + 2 * t) + s0 *      (x - x0)) + 
                       t  *      t  * (y1 * (3 - 2 * t) + s1 * dx * (t -  1));
        return y;
    }
private:
    static_assert(N >= 4, "makima must have more than 3 elements");
private:
    size_t len = 0;
    T x_[N] = {};
    T y_[N] = {};
    T d_[N] = {};
    bool is_valid = false;
};

}

#endif
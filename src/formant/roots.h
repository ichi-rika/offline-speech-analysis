#ifndef ROOTS_H
#define ROOTS_H

#include <complex>
#include "eigen.h"

namespace Roots
{
    template<typename Derived, typename T>
    void evaluatePoly(const DenseBase<Derived> &p, const T &x, T *y, T *dy)
    {
        int lenp = p.size();
        if (lenp < 1) {
            *y = *dy = 0;
        }
        else {
            *y = T(p(0));
            *dy = T(0);

            for (int k = 1; k < lenp; ++k) {
                *dy = *dy * x + *y;
                *y = *y * x + T(p(k));
            }
        }
    }

    template<typename Derived, typename T>
    void polishOne(const DenseBase<Derived> &poly, T *x, int maxit)
    {
        constexpr double eps = std::numeric_limits<double>::epsilon();

        T xbest = *x;
        double pmin = std::numeric_limits<double>::max();

        T p, dp;

        for (int i = 0; i < maxit; ++i) {
            evaluatePoly(poly, *x, &p, &dp);
            double absp = std::abs(p);
            if (absp > pmin || std::abs(absp - pmin) < eps) {
                *x = xbest;
                return;
            }
            pmin = absp;
            xbest = *x;
            if (std::abs(dp) == 0)
                return;
            *x -= p / dp;
        }
    }

    template<typename Derived, typename OtherDerived>
    void polish(const DenseBase<Derived> &p, DenseBase<OtherDerived> &r)
    {
        constexpr int maxit = 20;
        int i = 0;
        while (i < r.size()) {
            double re = r(i).real();
            double im = r(i).imag();

            if (im != 0) {
                polishOne(p, &r(i), maxit);
                if (i - 1 < r.size() && re == r(i + 1).real() && im == -r(i + 1).imag()) {
                    r(i + 1) = std::conj(r(i));
                    i++;
                }
            }
            else {
                polishOne(p, &re, maxit);
                r(i) = re;
            }

            i++;
        }
    }

    template<typename Derived>
    ArrayXcd solve(const DenseBase<Derived> &p)
    {
        const int n = p.size();

        MatrixXd c(n - 1, n - 1);
        c.leftCols<1>().setZero();
        c.bottomRows<1>() = -p.head(n - 1);
        c.topRightCorner(n - 2, n - 2).setIdentity();

        ArrayXcd r = c.eigenvalues();
        polish(p, r);
        return std::move(r);
    }
}

#endif // ROOTS_H

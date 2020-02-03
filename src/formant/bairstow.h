#ifndef BAIRSTOW_H
#define BAIRSTOW_H

#include <iostream>
#include "eigen.h"

namespace Bairstow
{
    void getRoots(double u, double v, double eps, std::vector<std::complex<double>> &roots);
    
    template<typename Derived>
    std::vector<std::complex<double>> solve(const DenseBase<Derived> &poly, double initialMag, double initialPha,
                                            int maxTotalIter = 50, int maxIter = 200,
                                            double eps1 = 0.001, double eps2 = 0.0001)
    {
        int n, iter, totalIter;
        double u, v, du, dv, denom;
        bool isCloseToZero;

        auto x = poly.reverse();

        n = x.size() - 1;
        ArrayXd b(n + 1);
        ArrayXd c(n + 1);

        std::vector<std::complex<double>> roots;

        // Pairs of roots.
        for (int nroot = 0; nroot < 2; ++nroot) {
            // Step 1
            totalIter = 0;
            iter = 0;
            u = -2 * initialMag * cos(initialPha);
            v = initialMag * initialMag;

            do {
                // Step 2
                b(0) = 1.0;
                b(1) = poly(1) - u;
                for (int i = 2; i <= n; ++i) {
                    b(i) = poly(i) - (b(i - 1) * u) - (b(i - 2) * v);
                }

                // Step 3
                c(0) = 1.0;
                c(1) = b(1) - u;
                for (int i = 2; i < n; ++i) {
                    c(i) = b(i) - (c(i - 1) * u) - (c(i - 2) * v);
                }

                // Step 4
                denom = c(n - 1) * c(n - 3) - c(n - 2) * c(n - 2);
                if (denom == 0) {
                    std::cerr << "bairstow: denominator is zero" << std::endl;
                    continue;
                }
                du = (b(n) * c(n - 3) - b(n - 1) * c(n - 2)) / denom;
                dv = (c(n - 1) * b(n - 1) - c(n - 2) * b(n)) / denom;

                // Step 5
                u += du;
                v += dv;
                if (iter < maxIter) {
                    iter++;
                } else {
                    iter = 0;
                    totalIter++;
                    if (totalIter >= maxTotalIter) {
                        break;
                    }
                    u = poly(n - 1) / poly(n - 2);
                    v = poly(n) / poly(n - 2);
                    continue;
                }
            } while (abs(du) > eps1 * abs(u) && abs(u) > eps2
                        && abs(dv) > eps1 * abs(v) && abs(v) > eps2);

            // Get roots.
            getRoots(u, v, eps1, roots);

            n -= 2;
            for (int i = 0; i <= n; ++i) {
                x(i) = b(i);
            }
        }

        return std::move(roots);
    }
}

#endif // BAIRSTOW_H

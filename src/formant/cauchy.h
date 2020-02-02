#ifndef CAUCHY_INTEGRAL_H
#define CAUCHY_INTEGRAL_H

#include <iterator>
#include <map>
#include <utility>
#include <vector>
#include "formant/roots.h"

namespace Cauchy
{
    template<typename Derived>
    void snellRegion(double t, std::map<double, int> &C, const DenseBase<Derived> &p, double phi)
    {
        if (C.find(t) == C.end()) {
            std::complex<double> x = std::polar(t, phi);
            std::complex<double> y, dy;
            Roots::evaluatePoly(p, x, &y, &dy);

            // Find the octant it is in.
            C[t] = (int) (16 + std::floor(std::arg(y) / (M_PI / 8.0))) % 8;
        }
    }

    template<typename Derived>
    std::vector<double> snellPartition(const std::vector<double> &t, std::map<double, int> &C, const DenseBase<Derived> &p, double phi, int maxDepth)
    {
        std::vector<std::pair<double, double>> partNext, partCurrent;
        for (int i = 0; i < t.size() - 1; ++i)
            partCurrent.emplace_back(t[i], t[i + 1]);

        bool converged = false;
        int depth = 0;

        while (!converged) {
            for (const auto& [t1, t2] : partCurrent) {
                snellRegion(t1, C, p, phi);
                snellRegion(t2, C, p, phi);

                int count = abs(C[t1] - C[t2]) % 8;
                if (count <= 1) {
                    partNext.emplace_back(t1, t2);
                }
                else {
                    double tmid = (t1 + t2) / 2;
                    partNext.emplace_back(t1, tmid);
                    partNext.emplace_back(tmid, t2);
                }
            }

            depth++;
            converged = depth > maxDepth || std::equal(partCurrent.begin(), partCurrent.end(), partNext.begin());
            partCurrent = std::move(partNext);
            partNext.clear();
        }

        std::vector<double> partition;
        partition.push_back(partCurrent.at(0).first);
        for (const auto& [t1, t2] : partCurrent) {
            partition.push_back(t2);
        }
        return std::move(partition);
    }

    template<typename Derived>
    int cauchyIntegral(const DenseBase<Derived> &p, double r1, double r2, double phi, int maxDepth)
    {
        // Let C(t) denote the region containing the point P(te^(j*phi)).
        // If ti, i = 0 to i = M is a partition of the ray (r1 -> r2),
        // then p(ti, ti+1) denotes the number of octants (mod 8) between C(ti) and C(ti-1)
        
        std::map<double, int> C;
        std::vector<double> partition = snellPartition({0, 0.2, 0.4, 0.6, 0.8, 1.0, 2.0},
                                                       C, p, phi, maxDepth);

        // Calculate N+ and N-
        //  N+ is the number of transitions from region C7 to region C0
        //  N- is the number of transitions from region C0 to region C7
        
        int Np = 0, Nm = 0;

        for (int i = 0; i < partition.size() - 1; ++i) {
            double t1 = partition[i];
            double t2 = partition[i + 1];

            snellRegion(t1, C, p, phi);
            snellRegion(t2, C, p, phi);

            int c1 = C[t1];
            int c2 = C[t2];

            if (c1 == 7 && c2 == 0)
                Np++;
            if (c1 == 0 && c2 == 7)
                Nm++;
        }

        return Np - Nm;
    }
}

#endif // CAUCHY_INTEGRAL_H

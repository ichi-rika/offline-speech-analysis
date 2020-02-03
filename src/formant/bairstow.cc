#include "formant/bairstow.h"

void Bairstow::getRoots(double u, double v, double eps, std::vector<std::complex<double>> &roots)
{
    double discr = u * u - 4 * v;
    if (std::abs(discr) < eps)
        discr = 0;

    if (discr == 0) {
        roots.push_back(-u / 2);
    }
    else if (discr > 0) {
        roots.push_back((-u + std::sqrt(discr)) / 2);
        roots.push_back((-u - std::sqrt(discr)) / 2);
    }
    else {
        roots.emplace_back(-u / 2, std::sqrt(-discr) / 2);
    }
}

#ifndef LPC_H
#define LPC_H

#include <vector>
#include "eigen.h"

namespace Lpc
{
    template<typename Derived, typename OtherDerived>
    double vecBurg(const DenseBase<Derived> &x, DenseBase<OtherDerived> &a)
    {
        int n = x.size();
        int m = a.size();
        a.setZero();

        ArrayXd b1 = ArrayXd::Zero(n + 1);
        ArrayXd b2 = ArrayXd::Zero(n + 1);
        ArrayXd aa = ArrayXd::Zero(m + 1);

        double p = 0.0;
        for (int j = 0; j < n; ++j)
            p += x(j) * x(j);
        
        double xms = p / (double) n;
        if (xms <= 0.0)
            return xms;

        b1(1) = x(0);
        b2(n - 1) = x(n - 1);
        for (int j = 2; j <= n; ++j)
            b1(j) = b2(j - 1) = x(j - 1);

        for (int i = 1; i <= m; ++i) {
            double num = 0.0;
            double den = 0.0;
            for (int j = 1; j <= n - i; ++j) {
                num += b1(j) * b2(j);
                den += b1(j) * b1(j) + b2(j) * b2(j);
            }

            if (den <= 0.0)
                return 0.0;

            a(i - 1) = (2.0 * num) / den;
            
            xms *= 1.0 - a(i - 1) * a(i - 1);

            for (int j = 1; j <= i - 1; ++j)
                a(j - 1) = aa(j) - a(i - 1) * aa(i - j);

            if (i < m) {
                for (int j = 1; j <= i; ++j)
                    aa(j) = a(j - 1);
                for (int j = 1; j <= n - i - 1; ++j) {
                    b1(j) -= aa(i) * b2(j);
                    b2(j) = b2(j + 1) - aa(i) * b1(j + 1);
                }
            }
        }

        return xms;
    }

    template<typename Derived>
    ArrayXd burg(const DenseBase<Derived> &x, int nCoefficients)
    {
        ArrayXd a(nCoefficients); 
        vecBurg(x, a);
        a = -a;
        return std::move(a);
    }

    template<typename Derived>
    std::vector<ArrayXd> analyse(const DenseBase<Derived> &x, double fs, int order, double frameLength, double frameSpace, double preEmphasisFrequency)
    {
        int sigLen = x.size();
        int winLen = round(frameLength / 1000.0 * fs);
        int hopSize = round(frameSpace / 1000.0 * fs);

        // Gaussian window
        ArrayXd win(winLen);
        double win_imid = 0.5 * (winLen + 1);
        double win_edge = std::exp(-12.0);
        for (int i = 1; i <= winLen; ++i) {
            win(i - 1) = (std::exp(-(48.0 * (i - win_imid) * (i - win_imid)) / (winLen + 1.0) / (winLen + 1.0)) - win_edge) / (1.0 - win_edge);
        }

        // Preemphasis.
        ArrayXd y = x;
        double y_a = std::exp(-2.0 * M_PI * preEmphasisFrequency / fs);
        for (int i = y.size() - 1; i >= 1; --i) {
            y(i) -= y_a * y(i - 1);
        }
        
        int start = 0;

        std::vector<ArrayXd> frames;

        while (start + winLen < y.size()) {
            ArrayXd lpcFrame = burg(y.segment(start, winLen) * win, order);

            frames.push_back(std::move(lpcFrame));

            start += hopSize;
        }

        return std::move(frames);
    }

}

#endif // LPC_H

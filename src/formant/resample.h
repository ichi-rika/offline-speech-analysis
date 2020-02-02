#ifndef RESAMPLE_H
#define RESAMPLE_H

#include <fftw3.h>
#include "eigen.h"

using namespace Eigen;

namespace Resample
{
    constexpr int Nearest = 0;
    constexpr int Linear = 1;
    constexpr int Cubic = 2;

    template<typename Derived>
    double interpolate_sinc(const DenseBase<Derived> &y, double x, int maxDepth)
    {
        int ix, midleft = std::floor(x), midright = midleft + 1, left, right;
        double result = 0.0, a, halfsina, aa, daa;

        // simple cases
        if (y.size() < 1) return NAN;
        if (x >= y.size()) return y(y.size() - 1);
        if (x < 0) return y(0);
        if (x == midleft) return y(midleft);
        // 1 < x < y.size && x not integer: interpolate
        if (maxDepth > midright - 1) maxDepth = midright - 1;
        if (maxDepth >= y.size() - midleft) maxDepth = y.size() - midleft - 1;
        if (maxDepth <= Resample::Nearest) return y((int) std::floor(x + 0.5));
        if (maxDepth == Resample::Linear) return y(midleft) + (x - midleft) * (y(midright) - y(midleft));
        if (maxDepth == Resample::Cubic) {
            double yl = y(midleft), yr = y(midright);
            double dyl = 0.5 * (yr - y(midleft - 1)), dyr = 0.5 * (y(midright+ 1) - yl);
            double fil = x - midleft, fir = midright - x;
            return yl * fir + yr * fil - fil * fir * (0.5 * (dyr - dyl) + (fil - 0.5) * (dyl + dyr - 2 * (yr - yl)));
        }

        left = midright - maxDepth;
        right = midleft + maxDepth;
        a = M_PI * (x - midleft);
        halfsina = 0.5 * std::sin(a);
        aa = a / (x - left + 1);
        daa = M_PI / (x - left + 1);
        for (ix = midleft; ix >= left; --ix) {
            double d = halfsina / a * (1.0 + std::cos(aa));
            result += y(ix) * d;
            a += M_PI;
            aa += daa;
            halfsina = -halfsina;
        }

        a = M_PI * (midright - x);
        halfsina = 0.5 * std::sin(a);
        aa = a / (right - x + 1);
        daa = M_PI / (right - x + 1);
        for (ix = midright; ix <= right; ++ix) {
            double d = halfsina / a * (1.0 + std::cos(aa));
            result += y(ix) * d;
            a += M_PI;
            aa += daa;
            halfsina = -halfsina;
        }

        return result;
    }
    
    template<typename Derived>
    ArrayXd resample(const DenseBase<Derived> &x, double sourceFs, double targetFs, int precision = 1)
    {
        double upfactor = targetFs / sourceFs;

        if (std::abs(upfactor - 1.0) < 1e-6) return x;

        int numberOfSamples = std::round((x.size() * targetFs) / sourceFs);
        if (numberOfSamples < 1) {
            return ArrayXd::Zero(0);
        }

        const int nx = x.size();
        ArrayXd z;
        if (upfactor < 1.0) {
            // Anti-aliasing filter.
            constexpr int antiTurnAround = 1000;
            constexpr int numberOfPaddingSides = 2;
            int nfft = 1;
            while (nfft < nx + antiTurnAround * numberOfPaddingSides) nfft *= 2;

            auto data = new double[nfft];
            auto data2 = new double[nfft];
            auto plf = fftw_plan_r2r_1d(nfft, data, data, FFTW_REDFT10, FFTW_MEASURE);
            auto plb = fftw_plan_r2r_1d(nfft, data2, data2, FFTW_REDFT01, FFTW_MEASURE);
        
            for (int i = 0; i < nfft; ++i) {
                if (i - antiTurnAround >= 0 && i - antiTurnAround < nx) {
                    data[i] = x[i - antiTurnAround];
                }
                else {
                    data[i] = 0;
                }
            }

            fftw_execute(plf);

            for (int i = std::floor(upfactor * nfft); i < nfft; ++i)
                data[i] = 0.0; // filter away high frequencies
            data[0] = 0.0;

            memcpy(data2, data, nfft * sizeof(double));

            fftw_execute(plb);
            
            z = Map<ArrayXd>(data2, nfft).segment(antiTurnAround, nx) / (double) (2 * nfft);
            
            fftw_destroy_plan(plf);
            fftw_destroy_plan(plb);
            delete[] data;
            delete[] data2;
        }
        else {
            z = x;
        }

        ArrayXd y(numberOfSamples);

        if (precision <= 1) {
            for (int i = 0; i < numberOfSamples; ++i) {
                double index = (i * sourceFs) / targetFs;
                int leftSample = std::floor(index);
                double fraction = index - leftSample;
                y(i) = (leftSample < 0 || leftSample >= nx - 1) ? 0.0 :
                        (1 - fraction) * z(leftSample) + fraction * z(leftSample + 1);
            }
        }
        else {
            for (int i = 0; i < numberOfSamples; ++i) {
                double index = (i * sourceFs) / targetFs;
                y(i) = interpolate_sinc(z, index, precision);
            }
        }

        return y;
    }


}

#endif // RESAMPLE_H

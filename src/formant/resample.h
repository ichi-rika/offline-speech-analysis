#ifndef RESAMPLE_H
#define RESAMPLE_H

#include <fftw3.h>
#include "eigen.h"

using namespace Eigen;

namespace Resample
{

    template<typename Derived>
    ArrayXd resample(const DenseBase<Derived> &x, double sourceFs, double targetFs)
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

        for (int i = 0; i < numberOfSamples; ++i) {
            double index = (i * sourceFs) / targetFs;
            int leftSample = std::floor(index);
            double fraction = index - leftSample;
            y(i) = (leftSample < 0 || leftSample >= nx - 1) ? 0.0 :
                    (1 - fraction) * z(leftSample) + fraction * z(leftSample + 1);
        }

        return y;

    }

}

#endif // RESAMPLE_H

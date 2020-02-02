#ifndef REAPER_H
#define REAPER_H

#include <iostream>
#include "eigen.h"
#include "main/track.h"
#include "reaper/compute.h"

namespace Reaper {

    template<typename Derived>
    int16_t *convertToSInt16(const DenseBase<Derived> &x)
    {
        using T = typename Derived::Scalar;
        constexpr T max(1.0);
        const int N = x.size();

        auto y = new int16_t[N];
        for (int i = 0; i < N; ++i) {
            y[i] = (int16_t) std::round(std::clamp(INT16_MAX * x(i) * max, T(INT16_MIN), T(INT16_MAX)));
        }
        return y;
    }

    template<typename Derived>
    void track(const DenseBase<Derived> &x, double fs, DblTrack &pitch, BoolTrack &voicing, double min_f0 = kMinF0Search, double max_f0 = kMaxF0Search)
    {
        constexpr bool do_hilbert_transform = true;

        EpochTracker et;
        et.set_unvoiced_cost(kUnvoicedCost);

        int16_t *wave_datap = convertToSInt16(x);
        int32_t n_samples = x.size();
        float sample_rate = fs;
        if (!et.Init(wave_datap, n_samples, sample_rate,
                     min_f0, max_f0, kDoHighpass, do_hilbert_transform)) {
            std::cerr << "Reaper: error in EpochTracker.Init()" << std::endl;
            delete[] wave_datap;
            exit(EXIT_FAILURE);
        }

        Track *f0 = nullptr;
        Track *pm = nullptr;
        Track *corr = nullptr;
        if (!ComputeEpochsAndF0(et, kUnvoicedPulseInterval, kExternalFrameInterval,
                                &pm, &f0, &corr)) {
            std::cerr << "Reaper: failed to compute epochs" << std::endl;
            delete[] wave_datap;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < f0->num_frames(); ++i) {
            const double t = f0->t(i);
            pitch[t] = f0->v(i) ? f0->a(i) : 0.0;
        }

        for (int i = 0; i < pm->num_frames(); ++i) {
            const double t = pm->t(i);
            voicing[t] = pm->v(i);
        }
    }

}

#endif // REAPER_H


#include <iostream>
#include "karma/karma.h"

VectorXd Karma::State::model()
{
    StateFormants state;
    unpack(state);

    VectorXd c(N);
    c.setZero();

    for (int i = 0; i < p/2; ++i) {
        const StateFormant &formant = state.formants.at(i);

        for (int n = 1; n <= N; ++n) {
            c(n - 1) += (exp(-M_PI * (double) n / fs * formant.bandwidth) / (double) n) *
                            cos(2 * M_PI * (double) n / fs * formant.frequency);
        }
    }

    c *= 2;

    return std::move(c);
}

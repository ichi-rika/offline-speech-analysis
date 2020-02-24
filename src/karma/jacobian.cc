#include "karma/karma.h"

MatrixXd Karma::State::jacobian()
{
    StateFormants state;
    unpack(state);

    MatrixXd H(N, p);

    for (int i = 0; i < p/2; ++i) {
        const StateFormant &formant = state.formants.at(i);
        
        for (int n = 1; n <= N; ++n) {
            H(n - 1, i) = -4 * M_PI * (exp(-M_PI * (double) n / fs * formant.bandwidth) / fs) *
                                        sin(2 * M_PI * (double) n / fs * formant.frequency);

            H(n - 1, p/2 + i) = -2 * M_PI * (exp(-M_PI * (double) n / fs * formant.bandwidth) / fs) *
                                          cos(2 * M_PI * (double) n / fs * formant.frequency);
        }
    }

    return std::move(H);
}

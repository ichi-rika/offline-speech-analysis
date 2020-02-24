#include <iostream>
#include "karma/karma.h"

std::vector<Karma::StateFormants> Karma::estimate(const ArrayXd &x, const BoolTrack &voicing, double fs, double frameDuration, double frameSpace, int I, int p, int N)
{
    int frameLength = round((fs * frameDuration) / 1000.0);
    
    if (frameLength >= x.size()) {
        std::cerr << "Input signal too short" << std::endl;
        return {};
    }

    ArrayXd win(frameLength);
    for (int n = 0; n < frameLength; ++n) {
        win(n) = 0.54 - 0.46 * cos(2 * M_PI * (double) n / (double) (frameLength - 1));
    }

    ArrayXd x_pre = x;
    for (int i = 1; i < x.size(); ++i) {
        x_pre(i) -= 0.7 * x_pre(i - 1);
    }

    int iframe = 0;
    int start = 0;

    State st;
    st.init(fs, I, p, N);

    std::vector<StateFormants> result;

    StateFormants state;
    StateFormants zero = {.formants = std::vector<StateFormant>(I)};
    
    do {
        ArrayXd frame = x_pre.segment(start, frameLength) * win;
        VectorXd a = arCoefficients(frame, p);
        VectorXd c = cepstrum(a, N);

        state.t = (frameSpace / 2.0 + iframe * frameSpace) / 1000.0;
       
        bool voiced = voicing.approx(state.t);

        st.step(c, voiced);
        
        if (!voiced) {
            zero.t = state.t;
            result.push_back(zero);
        }
        else {
            st.unpack(state);
            result.push_back(state);
        }

        iframe++;
        start = round((iframe * frameSpace * fs) / 1000.0);
    }
    while (start + frameLength < x.size()); 

    return std::move(result);
}

std::vector<DblTrack> Karma::toTrack(const std::vector<StateFormants> &frames, int I)
{
    const int I2 = frames.at(0).formants.size();

    std::vector<DblTrack> tracks(I2);
    std::vector<double> values(I2);

    for (int k = 0; k < frames.size(); ++k) {

        const StateFormants &state = frames.at(k);
        double t = state.t;

        for (int i = 0; i < I2; ++i) {
            double freq = state.formants.at(i).frequency;
            values[i] = std::isnormal(freq) ? freq : 0;
        }

        std::sort(values.begin(), values.end());

        for (int i = 0; i < I; ++i) {
            tracks.at(i)[t] = values[i];
        }

    }

    return std::move(tracks);
}

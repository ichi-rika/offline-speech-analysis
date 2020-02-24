#include <iostream>
#include "karma/karma.h"

void Karma::State::unpack(StateFormants &state)
{
    state.formants.resize(p/2);
    
    for (int i = 0; i < p/2; ++i) {
        StateFormant &formant = state.formants.at(i);

        formant.frequency = m(i);
        formant.bandwidth = m(p/2 + i);
    }
}

void Karma::State::pack(const StateFormants &state)
{
    m.resize(p);

    for (int i = 0; i < p/2; ++i) {
        const StateFormant &formant = state.formants.at(i);

        m(i) = formant.frequency;
        m(p/2 + i) = formant.bandwidth;
    }
}

void Karma::State::init(double fs, int I, int p, int N)
{
    this->fs = fs;
    this->I = I;
    this->p = p;
    this->N = N;

    if (I > 4) {
        std::cerr << "Number of formants must be lesser than or equal to 4" << std::endl;
        return;
    }

    if (p < 2 * I) {
        std::cerr << "AR order must be greater than or equal to " << (2 * I) << std::endl;
        return;
    }

    if (N < p) {
        std::cerr << "Cepstrum order must be greater than or equal to " << p << std::endl;
    }

    StateFormants state;
    state.formants.resize(p/2);
    for (int i = 0; i < p/2; ++i) {
        StateFormant &formant = state.formants.at(i);

        switch (i) {
        case 0:
            formant.frequency = 500;
            formant.bandwidth = 80;
            break;
        case 1:
            formant.frequency = 1500;
            formant.bandwidth = 120;
            break;
        case 2:
            formant.frequency = 2500;
            formant.bandwidth = 160;
            break;
        case 3:
            formant.frequency = 3500;
            formant.bandwidth = 180;
            break;
        default:
            formant.frequency = 0;
            formant.bandwidth = 15;
            break;
        }
    }
    pack(state);

    F.setIdentity(p, p);

    Q.setZero(p, p);
    for (int i = 0; i < p/2; ++i) {
        Q(i, i) = 320 * 320;
        Q(p/2 + i, p/2 + i) = 100 * 100;
    }

    R.setZero(N, N);
    for (int n = 1; n <= N; ++n) {
        R(n - 1, n - 1) = 1.0 / (double) n;
    }

    P = Q;
}

void Karma::State::step(const VectorXd &yt, bool voiced)
{
    VectorXd m1 = F * m;
    MatrixXd P1 = F * P * F.transpose() + Q;

    if (voiced) {
        MatrixXd Ht = jacobian();
        MatrixXd Kt = P1 * Ht.transpose() * (Ht * P1 * Ht.transpose() + R).inverse();

        VectorXd hm1 = model();
        m = m1 + Kt * (yt - hm1);
        P = P1 - Kt * Ht * P1;
    }
    else {
        m = m1;
        P = P1;
    }
}

#ifndef KARMA_H
#define KARMA_H

#include "eigen.h"
#include "main/track.h"

namespace Karma
{
    struct StateFormant
    {
        StateFormant() : frequency(0), bandwidth(0) {}

        double frequency;
        double bandwidth;
    };

    struct StateFormants
    {
        double t;
        std::vector<StateFormant> formants;
    };

    struct State
    {
        int I, p, N;
        double fs;
        VectorXd m; // 1 x p
        MatrixXd F; // p x p
        MatrixXd Q; // p x p
        MatrixXd R; // N x N
        MatrixXd P; // N x N

        void unpack(StateFormants &state);
        void pack(const StateFormants &state);
   
        void init(double fs, int I, int p, int N);
        void step(const VectorXd &y, bool voiced);

    private: 
        VectorXd model();
        MatrixXd jacobian();
    };

    VectorXd arCoefficients(const ArrayXd &x, int p);
    VectorXd cepstrum(const VectorXd &a, int N);

    std::vector<StateFormants> estimate(const ArrayXd &x, const BoolTrack &voicing, double fs, double frameDuration, double frameSpace, int I, int p, int N);

    std::vector<DblTrack> toTrack(const std::vector<StateFormants> &frames, int I);
    
}

#endif // KARMA_H

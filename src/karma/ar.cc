#include "karma/karma.h"
#include "formant/lpc.h"

VectorXd Karma::arCoefficients(const ArrayXd &x, int p)
{
    return -Lpc::burg(x, p);
}

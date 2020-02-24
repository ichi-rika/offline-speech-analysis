#include "karma/karma.h"

VectorXd Karma::cepstrum(const VectorXd &a, int N)
{
    const int p = a.size();
    VectorXd c(N);

    for (int n = 1; n <= N; ++n) {
        
        if (n == 1) {
            c(n - 1) = a(n - 1);
        }
        else if (n > 1 && n <= p) {
            c(n - 1) = 0;
            for (int i = 1; i <= n - 1; ++i) {
                c(n - 1) += ((double) i / (double) n) * a(n - i - 1) * c(i - 1);
            }
            c(n - 1) += a(n - 1);
        }
        else if (n > p) {
            c(n - 1) = 0;
            for (int i = n - p; i <= n - 1; ++i) {
                c(n - 1) += ((double) i / (double) n) * a(n - i - 1) * c(i - 1);
            }
        }

    }

    return std::move(c);
}

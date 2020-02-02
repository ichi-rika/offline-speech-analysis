#ifndef FORMANT_H
#define FORMANT_H

#include "eigen.h"
#include "formant/bairstow.h"
#include "formant/lpc.h"
#include "formant/cauchy.h"

namespace Formant
{
    struct frm_root {
        double r, phi;
        double f, b;
    };

    template<typename Derived, typename OtherDerived>
    std::vector<frm_root> fromRoots(const DenseBase<Derived> &p, const DenseBase<OtherDerived> &r, double fs)
    {
        std::vector<frm_root> roots;
        std::vector<frm_root> peakMergers;
        std::vector<frm_root> finalRoots;

        for (const auto &v : r) {
            if (v.imag() < 0)
                continue;
            
            double r = std::abs(v);
            if (0.7 <= r && r < 1.0) {
                double phi = std::arg(v);
                double f = std::abs(phi) * fs / (2.0 * M_PI);
                if (f > 70) {
                    double b = -std::log(r) * fs / M_PI;
                    roots.push_back({r, phi, f, b});
                }
            }
        }

        std::sort(roots.begin(), roots.end(),
                [](const auto &a, const auto &b) { return a.f < b.f; });

        int nCand = roots.size();
        for (int i = 0; i < nCand; ++i) {
            double f1 = roots[i].f;

            if (i < nCand - 1) {
                double f2 = roots[i + 1].f;
                if (std::abs(f2 - f1) > 700) {
                    peakMergers.push_back(std::move(roots[i]));
                }
                else if (i == 0 && f2 > 1800) {
                    peakMergers.push_back(std::move(roots[i]));
                }
            }
            else {
                finalRoots.push_back(std::move(roots[i]));
            }
        }

        for (const auto &v : peakMergers) {
            double phiPeak = v.phi;
            double deltaPhi = 700 * 2 * M_PI / fs;

            double phi3 = phiPeak - deltaPhi / 2;
            double phi4 = phiPeak + deltaPhi / 2;

            double minPhi = 200 * 2 * M_PI / fs;
            if (phi3 < minPhi)
                phi3 = minPhi;

            int n3 = Cauchy::cauchyIntegral(p, 0, 2, phi3, 10);
            int n4 = Cauchy::cauchyIntegral(p, 0, 2, phi4, 10);
            int n = std::abs(n4 - n3);

            if (n >= 2) {
                std::vector<std::complex<double>> polished = Bairstow::solve(p, 0.9, phiPeak);
                for (const auto &w : polished) {
                    double r = std::abs(w);
                    if (0.7 <= r && r < 1.0) {
                        double phi = std::arg(w);
                        double f = std::abs(phi) * fs / (2.0 * M_PI);
                        if (f > 70) {
                            double b = -std::log(r) * fs / M_PI;
                            finalRoots.push_back({r, phi, f, b});
                        }
                    }
                }
            }
            else {
                finalRoots.push_back(v);
            }
        }

        std::sort(finalRoots.begin(), finalRoots.end(),
                [](const auto &a, const auto &b) { return a.f < b.f; });

        return std::move(finalRoots);
    }

    template<typename Derived>
    std::vector<frm_root> fromLpc(const DenseBase<Derived> &a, double fs)
    {
        if (a.size() > 0) {
            ArrayXd p(a.size() + 1);
            p(0) = 1;
            p.tail(a.size()) = a;

            ArrayXcd r = Roots::solve(p);
            return fromRoots(p, r, fs); 
        }

        return std::vector<frm_root>();
    }

    template<typename Derived>
    std::vector<std::vector<frm_root>> analyse(const DenseBase<Derived> &x, double fs, int order, double frameLength, double frameSpace, double preEmphasisFrequency)
    {
        std::vector<ArrayXd> lpc = Lpc::analyse(x, fs, order, frameLength, frameSpace, preEmphasisFrequency);
        std::vector<std::vector<frm_root>> frms;

        for (const auto &a : lpc) {
            frms.push_back(fromLpc(a, fs));
        }

        return std::move(frms);
    }

    template<typename Derived>
    std::vector<DblTrack> track(const DenseBase<Derived> &x, double fs, int order, double frameLength, double frameSpace, double preEmphasisFrequency)
    {
        std::vector<std::vector<frm_root>> frms = analyse(x, fs, order, frameLength, frameSpace, preEmphasisFrequency);
        std::vector<DblTrack> tracks;

        double t = frameLength / 1000 / 2;

        for (const auto &frm : frms) {
            int n = 0;
            for (const auto &v : frm) {
                if (n >= tracks.size()) {
                    tracks.emplace_back();
                }

                tracks[n][t] = v.f;
                n++;
            }

            t += frameSpace / 1000;
        }

        return std::move(tracks);
    }

}

#endif // FORMANT_H

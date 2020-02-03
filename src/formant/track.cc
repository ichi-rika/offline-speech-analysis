#include <iostream>
#include "formant/formant.h"
#include "formant/viterbi.h"

using Formant::frm_root;

struct fparm {
    const std::vector<std::vector<frm_root>> *me;
    std::vector<std::vector<frm_root>> *thee;
    double dfCost, bfCost, octaveJumpCost;
    double refF[5];
};

static double getLocalCost(int iframe, int icand, int itrack, void *closure)
{
    const auto parm = (struct fparm *) closure;
    const auto &frm = parm->me->at(iframe-1);
    if (icand > frm.size()) return 1e30;
    const auto &cand = frm.at(icand-1);
    return parm->dfCost * std::abs(cand.f - parm->refF[itrack-1])
            + parm->bfCost * cand.b / cand.f;
}

static double getTransitionCost(int iframe, int icand1, int icand2, int itrack, void *closure)
{
    const auto parm = (struct fparm *) closure;
    const auto &prevFrm = parm->me->at(iframe-1 - 1);
    const auto &curFrm = parm->me->at(iframe-1);
    if (icand1 > prevFrm.size() || icand2 > curFrm.size()) return 1e30;
    const double f1 = prevFrm.at(icand1-1).f;
    const double f2 = curFrm.at(icand2-1).f;
    return parm->octaveJumpCost * std::abs(std::log2(f1 / f2));
}

static void putResult(int iframe, int place, int itrack, void *closure)
{
    auto prm = (struct fparm *) closure;

    if (iframe > prm->thee->size()
            || itrack > prm->thee->at(iframe-1).size()
            || place > prm->me->at(iframe-1).size())
        return;

    prm->thee->at(iframe-1).at(itrack-1) = prm->me->at(iframe-1).at(place-1);
}

std::vector<DblTrack> Formant::track(
        const std::vector<std::vector<frm_root>> &frms,
        double frameLength, double frameSpace,
        int ntrack,
        double refF1, double refF2, double refF3, double refF4, double refF5,
        double dfCost, double bfCost, double octaveJumpCost)
{
    int nFrmMin = std::numeric_limits<int>::max();
    int nFrmMax = 0;
    for (const auto &frm : frms) {
        nFrmMin = std::min<int>(nFrmMin, frm.size());
        nFrmMax = std::max<int>(nFrmMax, frm.size());
    }

    /*if (ntrack > nFrmMin) {
        std::cerr << "Formant: number of tracks (" << ntrack << ") is greater than the minimum number of formants (" << nFrmMin << ")" << std::endl;
        return {};
    }*/

    int nframe = frms.size();
    std::vector<std::vector<frm_root>> outFrms(nframe);
    for (int i = 0; i < nframe; ++i) {
        outFrms.at(i).resize(ntrack, {.f = 0});
    }

    struct fparm parm;
    parm.me = &frms;
    parm.thee = &outFrms;
    parm.dfCost = dfCost / 1000.0;
    parm.bfCost = bfCost;
    parm.octaveJumpCost = octaveJumpCost;
    parm.refF[0] = refF1;
    parm.refF[1] = refF2;
    parm.refF[2] = refF3;
    parm.refF[3] = refF4;
    parm.refF[4] = refF5;

    Viterbi::viterbiMulti(nframe, nFrmMax, ntrack,
                          &getLocalCost, &getTransitionCost, &putResult,
                          &parm);

    std::vector<DblTrack> tracks(ntrack);
   
    for (int iframe = 0; iframe < nframe; ++iframe) {
        double t = (frameLength / 2.0 + iframe * frameSpace) / 1000.0;
        const auto &frm = outFrms.at(iframe);
        for (int itrack = 0; itrack < frm.size(); ++itrack) {
            tracks.at(itrack)[t] = frm.at(itrack).f;
        }
    }

    return std::move(tracks);
}

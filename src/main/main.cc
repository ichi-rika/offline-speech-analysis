#include <QWidget>
#include "eigen.h"
#include "main/track.h"
#include "gui/gui.h"
#include "audio/audio.h"
#include "formant/formant.h"
#include "formant/resample.h"
#include "reaper/reaper.h"

int main(int argc, char **argv)
{
    int arglen;
    for (int i = 1; i < argc; ++i) {
        arglen += strlen(argv[i]);
        if (i < argc - 1)
            arglen++;
    }

    char argstr[arglen];
    argstr[0] = '\0';
    for (int i = 1; i < argc; ++i) {
        strcat(argstr, argv[i]);
        if (i < argc - 1)
            strcat(argstr, " ");
    }

    double *data;
    int fs = 44100;
    int length = Audio::decodeFile(argstr, fs, &data);
    if (length < 0) {
        std::cerr << "Could not decode file" << std::endl;
        return EXIT_FAILURE;
    }

    double duration = (double) length / (double) fs;

    Map<ArrayXd> x(data, length);

    DblTrack pitch;
    BoolTrack voicing;
    
    Reaper::track(x, fs, pitch, voicing);

    ArrayXd x_res = Resample::resample(x, fs, 12000, 50);
    
    std::vector<std::vector<Formant::frm_root>> rawFormants = Formant::analyse(
            x_res, 12000,
            10,
            25.0, 0.5,
            500.0
    );

    std::vector<DblTrack> formantTracks = Formant::track(
            rawFormants,
            25.0, 0.5,
            5,
            550,
            1650,
            2750,
            3850,
            4950,
            1.0,
            0.8,
            1.5
    );

    //std::vector<DblTrack> formantTracks = Formant::analyseTracks(x_res, 12000, 12, 25.0, 0.5, 500.0);

    QApplication app(argc, argv);

    Style::init(app);

    MainWindow w;
    w.trackView->addTrack(pitch, Qt::cyan);

    QColor trackColors[] = {
        "orange",
        "pink",
        "green",
        "lightblue",
        "yellow",
    };
    int itrack = 0;
    for (const auto &track : formantTracks) {
        w.trackView->addTrack(track, trackColors[itrack], true);
        itrack++;
    }

    w.trackView->setFrequencyRange(0, 5000);
    w.trackView->setTimeRange(0, duration);
    w.show();

    return app.exec();
}

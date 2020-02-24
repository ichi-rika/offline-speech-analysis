#include <QWidget>
#include "eigen.h"
#include "main/track.h"
#include "gui/gui.h"
#include "audio/audio.h"
#include "formant/formant.h"
#include "formant/resample.h"
#include "karma/karma.h"
#include "reaper/reaper.h"

int main(int argc, char **argv)
{
    int arglen = 0;
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

    ArrayXd x_karma = Resample::resample(x, fs, 7000, 10);
    
    std::vector<Karma::StateFormants> formants = Karma::estimate(x_karma, voicing, 7000, 20.0, 10.0, 3, 12, 15);
    std::vector<DblTrack> formantTracks = Karma::toTrack(formants, 3);

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
        w.trackView->addTrack(track, trackColors[itrack], false);
        itrack++;
    }

    w.trackView->setFrequencyRange(0, 4500);
    w.trackView->setTimeRange(0, duration);
    w.show();

    return app.exec();
}

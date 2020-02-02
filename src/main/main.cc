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

    ArrayXd x_res = Resample::resample(x, fs, 10000, 50);
    std::vector<DblTrack> frmTracks = Formant::track(x_res, 10000, 10, 35.0, 1.0, 100.0);

    QApplication app(argc, argv);

    Style::init(app);

    MainWindow w;
    w.trackView->addTrack(pitch, Qt::cyan);

    for (const auto &track : frmTracks) {
        w.trackView->addTrack(track, "orange");
    }

    w.trackView->setFrequencyRange(0, 4000);
    w.trackView->setTimeRange(0, duration);
    w.show();

    return app.exec();
}

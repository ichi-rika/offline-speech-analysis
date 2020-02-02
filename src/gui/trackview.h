#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QWidget>
#include "main/track.h"
#include "plot/graph.h"

class TrackView : public QWidget
{
public:
    TrackView(QWidget *parent = nullptr);
    ~TrackView();

    void addTrack(const DblTrack &track, const QColor &color);
    void removeAllTracks();
    
    void setFrequencyRange(double min, double max);
    void setTimeRange(double min, double max);

private:
    PlotGraph *m_graph;
};

#endif // TRACKVIEW_H

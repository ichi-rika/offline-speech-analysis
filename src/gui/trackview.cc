#include <QBoxLayout>
#include <QLabel>
#include "gui/trackview.h"
#include "plot/axis.h"
#include "plot/series.h"

TrackView::TrackView(QWidget *parent)
    : QWidget(parent), m_graph(new PlotGraph)
{
    auto ly1 = new QVBoxLayout;
    setLayout(ly1);

    ly1->addWidget(m_graph);
}

TrackView::~TrackView()
{
    delete m_graph;
}

void TrackView::addTrack(const DblTrack &track, bool scatter)
{
    auto series = new PlotSeries(scatter);
    for (const auto [x, y] : track) {
        series->append(x, y);
    }
    m_graph->addSeries(series);
}

void TrackView::removeAllTracks()
{
    m_graph->removeAllSeries();
}

void TrackView::setFrequencyRange(double min, double max)
{
    m_graph->yAxis()->setRange(min, max);
}

void TrackView::setTimeRange(double min, double max)
{
    m_graph->xAxis()->setRange(min, max);
}

#include "plot/graph.h"
#include "plot/series.h"
#include "plot/axis.h"

PlotGraph::PlotGraph(QWidget *parent)
    : QWidget(parent), m_xAxis(new PlotAxis), m_yAxis(new PlotAxis)
{
    m_yAxis->setDataType(Mel);
    m_yAxis->setAxisType(Reverse);
}

PlotGraph::~PlotGraph()
{
    removeAllSeries();
    delete m_xAxis;
    delete m_yAxis;
}

void PlotGraph::addSeries(PlotSeries *series)
{
    m_seriesList.append(series);
}

void PlotGraph::removeAllSeries()
{
    for (PlotSeries *series : m_seriesList)
        delete series;
    m_seriesList.clear();
}

PlotAxis *PlotGraph::xAxis()
{
    return m_xAxis;
}

PlotAxis *PlotGraph::yAxis()
{
    return m_yAxis;
}

void PlotGraph::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    m_xAxis->render(painter);
    m_yAxis->render(painter);

    for (PlotSeries *series : m_seriesList) {
        series->render(m_xAxis, m_yAxis, painter);
    }
}

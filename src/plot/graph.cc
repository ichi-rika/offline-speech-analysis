#include "plot/graph.h"
#include "plot/series.h"
#include "plot/axis.h"

PlotGraph::PlotGraph(QWidget *parent)
    : QWidget(parent), m_xAxis(new PlotAxis(Horizontal)), m_yAxis(new PlotAxis(Vertical))
{
    m_xAxis->setTitle("Time (s)");
    m_xAxis->setTickSpacing(1.0);
    m_xAxis->setMinorTickCount(10);
    m_xAxis->setDataType(Linear);

    m_yAxis->setTitle("Frequency (Hz)");
    m_yAxis->setTickSpacing(1000.0);
    m_yAxis->setMinorTickCount(10);
    m_yAxis->setDataType(Mel);
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

    int marginLeft = m_yAxis->width(painter);
    int marginBottom = m_xAxis->width(painter);

    for (PlotSeries *series : m_seriesList) {
        series->render(marginLeft, marginBottom, m_xAxis, m_yAxis, painter);
    }
    
    m_xAxis->render(marginLeft, painter);
    m_yAxis->render(marginBottom, painter);
}

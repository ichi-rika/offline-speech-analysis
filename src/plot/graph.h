#ifndef PLOT_GRAPH_H
#define PLOT_GRAPH_H

#include <QWidget>
#include <QList>

class PlotAxis;
class PlotSeries;

class PlotGraph : public QWidget
{
public:
    PlotGraph(QWidget *parent = nullptr);
    ~PlotGraph();

    void addSeries(PlotSeries *series);
    void removeAllSeries();

    PlotAxis *xAxis();
    PlotAxis *yAxis();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QList<PlotSeries *> m_seriesList;
    PlotAxis *m_xAxis;
    PlotAxis *m_yAxis;
};

#endif // PLOT_GRAPH_H

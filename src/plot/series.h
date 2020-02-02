#ifndef PLOT_SERIES_H
#define PLOT_SERIES_H

#include <QVector>
#include <QPainter>

class PlotAxis;

class PlotSeries
{
public:
    PlotSeries(bool scatter = false);

    void append(double x, double y);

    void render(PlotAxis *xAxis, PlotAxis *yAxis, QPainter &painter) const;

private:
    bool m_scatter;
    int m_nElt;
    QVector<double> m_xs, m_ys;
};

#endif // PLOT_SERIES_H


#ifndef PLOT_SERIES_H
#define PLOT_SERIES_H

#include <QVector>
#include <QPainter>

class PlotAxis;

class PlotSeries
{
public:
    PlotSeries(const QColor &color, bool scatter);

    void append(double x, double y);

    void render(int marginLeft, int marginBottom, PlotAxis *xAxis, PlotAxis *yAxis, QPainter &painter) const;

private:
    QColor m_color;
    bool m_scatter;
    int m_nElt;
    QVector<double> m_xs, m_ys;
};

#endif // PLOT_SERIES_H


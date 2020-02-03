#include "plot/series.h"
#include "plot/axis.h"

PlotSeries::PlotSeries(const QColor &color, bool scatter)
    : m_color(color), m_scatter(scatter), m_nElt(0)
{
}

void PlotSeries::append(double x, double y)
{
    m_xs.append(x);
    m_ys.append(y);
    m_nElt++;
}

void PlotSeries::render(int marginLeft, int marginBottom, PlotAxis *xAxis, PlotAxis *yAxis, QPainter &painter) const
{
    QRect r = painter.window();
    int w = r.width() - marginLeft;
    int h = r.height() - marginBottom;

    int x, y;

    if (!m_scatter) {
        QPainterPath path;

        x = xAxis->realToPlot(w, m_xs[0]) + marginLeft;
        y = yAxis->realToPlot(h, m_ys[0]);

        path.moveTo(x, y);

        for (int i = 1; i < m_nElt; ++i) { 
            x = xAxis->realToPlot(w, m_xs[i]) + marginLeft;
            y = yAxis->realToPlot(h, m_ys[i]);
          
            if (m_ys[i] <= 0) {
                continue;
            }

            if (m_ys[i - 1] <= 0 && m_ys[i] > 0) {
                path.moveTo(x, y);
            }
            else {
                path.lineTo(x, y);
            }
        }

        painter.setPen(QPen(m_color, 2));
        painter.drawPath(path);
    }
    else {
        QBrush b(m_color);
        
        for (int i = 0; i < m_nElt; ++i) {
            x = xAxis->realToPlot(w, m_xs[i]) + marginLeft;
            y = yAxis->realToPlot(h, m_ys[i]);

            painter.fillRect(x, y - 1, 1, 2, b);
        }
    }
}

#include "plot/axis.h"

PlotAxis::PlotAxis(AxisOrientation orientation)
    : m_orientation(orientation)
{
    setRange(0, 100);
    setTitle("");
    setTypes(Linear, orientation == Horizontal ? Normal : Reverse);
    setTickSpacing(100.0);
    setMinorTickCount(10);
}

double PlotAxis::min() const
{
    return m_min;
}

double PlotAxis::max() const
{
    return m_max;
}

void PlotAxis::setMin(double min)
{
    m_min = min;
}

void PlotAxis::setMax(double max)
{
    m_max = max;
}

void PlotAxis::setRange(double min, double max)
{
    m_min = min;
    m_max = max;
}

QString PlotAxis::title() const
{
    return m_title;
}

void PlotAxis::setTitle(const QString &title)
{
    m_title = title;
}

double PlotAxis::tickSpacing() const
{
    return m_tickSpacing;
}

int PlotAxis::minorTickCount() const
{
    return m_minorTickCount;
}

void PlotAxis::setTickSpacing(double spacing)
{
    m_tickSpacing = spacing;
}

void PlotAxis::setMinorTickCount(int minorCount)
{
    m_minorTickCount = minorCount;
}

void PlotAxis::setDataType(DataTransform type)
{
    m_dataTrans = PlotTransform::data(type);
}

void PlotAxis::setAxisType(AxisTransform type)
{
    m_axisTrans = PlotTransform::axis(type);
}

void PlotAxis::setTypes(DataTransform dataType, AxisTransform axisType)
{
    m_dataTrans = PlotTransform::data(dataType);
    m_axisTrans = PlotTransform::axis(axisType);
}

int PlotAxis::width(QPainter &painter) const
{
    QRect titleRect = painter.fontMetrics().boundingRect(m_title);

    if (m_orientation == Horizontal) {
        return titleRect.height() + 2;
    }
    else {
        return titleRect.width() + 2 + 8;
    }
}

void PlotAxis::render(int margin, QPainter &painter) const
{
    QRect r = painter.window();
    int w = r.width();
    int h = r.height();

    painter.setPen(QPen(Qt::white, 2));

    QRect titleRect = painter.fontMetrics().boundingRect(m_title);
    
    if (m_orientation == Horizontal) {

        painter.drawText(w - 2 - titleRect.width(), h - 2, m_title);

        int yline = h - 2 - titleRect.height();

        painter.drawLine(margin - 2, yline, w, yline);

        double minTick = std::floor(m_min / m_tickSpacing) * m_tickSpacing;
        double maxTick = std::ceil(m_max / m_tickSpacing) * m_tickSpacing;

        double tick1 = minTick;
        double tick2 = tick1 + m_tickSpacing;

        int x;

        while (tick2 <= maxTick) {
            
            if (tick1 >= m_min) {
                x = margin + realToPlot(w - margin, tick1);
                painter.drawLine(x, yline, x, yline + 8);

                QRect tickRect = painter.fontMetrics().boundingRect(QString::number(tick1));
                painter.drawText(x - tickRect.width() / 2, yline + 2 + tickRect.height(), QString::number(tick1));

                for (int i = 1; i <= m_minorTickCount; ++i) {
                    double tick = tick1 + ((i * m_tickSpacing) / m_minorTickCount);
                    x = margin + realToPlot(w - margin, tick);
                    
                    painter.drawLine(x, yline, x, yline + 4);

                    if (i % 2 == 0) {
                        QRect tickRect = painter.fontMetrics().boundingRect(QString::number(tick));
                        painter.drawText(x - tickRect.width() / 2, yline + 2 + tickRect.height(), QString::number(tick));
                    }
                }
            }

            tick1 = tick2;
            tick2 += m_tickSpacing;
        }
    }
    else {

        painter.drawText(2, 2 + titleRect.height(), m_title);

        int xline = 2 + titleRect.width() + 8;

        painter.drawLine(xline, 0, xline, h - margin + 2);
        
        double minTick = std::floor(m_min / m_tickSpacing) * m_tickSpacing;
        double maxTick = std::ceil(m_max / m_tickSpacing) * m_tickSpacing;

        double tick1 = minTick;
        double tick2 = tick1 + m_tickSpacing;

        int y;

        while (tick2 <= maxTick) {
            
            if (tick1 >= m_min) {
                y = realToPlot(h - margin, tick1);
                painter.drawLine(xline, y, xline - 8, y);

                QRect tickRect = painter.fontMetrics().boundingRect(QString::number(tick1));
                painter.drawText(xline - 8 - tickRect.width() - 2, y + tickRect.height() / 4, QString::number(tick1));

                for (int i = 1; i < m_minorTickCount; ++i) {
                    double tick = tick1 + ((i * m_tickSpacing) / m_minorTickCount);
                    y = realToPlot(h - margin, tick);
                    
                    painter.drawLine(xline, y, xline - 4, y);
               
                    if (i % 2 == 0) {
                        QRect tickRect = painter.fontMetrics().boundingRect(QString::number(tick));
                        painter.drawText(xline - 8 - tickRect.width() - 2, y + tickRect.height() / 4, QString::number(tick));
                    }
                }
            }

            tick1 = tick2;
            tick2 += m_tickSpacing;
        }
    }

}

int PlotAxis::realToPlot(int length, double xr) const
{
    double x = m_dataTrans->forward(m_min, m_max, xr);
    return m_axisTrans->forward(0, length, x);
}

double PlotAxis::plotToReal(int length, int xp) const
{
    double x = m_axisTrans->backward(0, length, xp);
    return m_dataTrans->backward(m_min, m_max, x);
}


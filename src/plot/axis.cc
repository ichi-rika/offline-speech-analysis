#include "plot/axis.h"

PlotAxis::PlotAxis()
{
    setRange(0, 100);
    setTitle("");
    setTypes(Linear, Normal);
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
    setDataType(dataType);
    setAxisType(axisType);
}

void PlotAxis::render(QPainter &painter) const
{

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

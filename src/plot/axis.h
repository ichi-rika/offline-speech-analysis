#ifndef PLOT_AXIS_H
#define PLOT_AXIS_H

#include <QPainter>

#include "plot/transform.h"

class PlotAxis
{
public:
    PlotAxis();

    double min() const;
    double max() const;
    void setMin(double min);
    void setMax(double max);
    void setRange(double min, double max);

    QString title() const;
    void setTitle(const QString &title);

    void setDataType(DataTransform type);
    void setAxisType(AxisTransform type);
    void setTypes(DataTransform dataType, AxisTransform axisType);

    void render(QPainter &painter) const;
    
    // Between data domain and axis domain
    int realToPlot(int length, double xr) const;
    double plotToReal(int length, int xp) const;

protected:

    double m_min;
    double m_max;
    QString m_title;

    // Between data domain and [0,1]
    PlotTransform *m_dataTrans;

    // Between [0,1] and axis domain
    PlotTransform *m_axisTrans;
};

#endif // PLOT_AXIS_H

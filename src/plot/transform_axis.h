#ifndef PLOT_TRANSFORM_AXIS_H
#define PLOT_TRANSFORM_AXIS_H

class AxisTransformNormal : public PlotTransform
{
public:
    inline virtual double forward(double min, double max, double x) const
    {
        return round(min + x * (max - min));
    }

    inline virtual double backward(double min, double max, double x) const
    {
        return (x - min) / (max - min);
    }
};

class AxisTransformReverse : public PlotTransform
{
public:
    inline virtual double forward(double min, double max, double x) const
    {
        return round(min + (1 - x) * (max - min));
    }

    inline virtual double backward(double min, double max, double x) const
    {
        return (x - min) / (min - max);
    }
};

#endif // PLOT_TRANSFORM_AXIS_H

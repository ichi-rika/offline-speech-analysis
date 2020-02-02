#ifndef PLOT_TRANSFORM_H
#define PLOT_TRANSFORM_H

#include <math.h>
#include <array>
#include <memory>

enum DataTransform {
    Linear = 0, Logarithmic, Mel, Bark,
    dataTransformCount // DO NOT USE!!
};

enum AxisTransform {
    Normal = 0, Reverse,
    axisTransformCount // DO NOT USE!!
};

class PlotTransform
{
public:
    virtual double forward(double min, double max, double x) const = 0;
    virtual double backward(double min, double max, double x) const = 0;

    static PlotTransform *data(DataTransform type);
    static PlotTransform *axis(AxisTransform type);

private:
    static std::array<std::shared_ptr<PlotTransform>, dataTransformCount> dataTransforms;
    static std::array<std::shared_ptr<PlotTransform>, axisTransformCount> axisTransforms;
    static bool hasBeenInit;
    static void initInstances();
};

#include "plot/transform_data.h"
#include "plot/transform_axis.h"

#endif // PLOT_TRANSFORM_H

#include "plot/transform.h"

std::array<std::shared_ptr<PlotTransform>, dataTransformCount> PlotTransform::dataTransforms;
std::array<std::shared_ptr<PlotTransform>, axisTransformCount> PlotTransform::axisTransforms;

bool PlotTransform::hasBeenInit = false;

void PlotTransform::initInstances()
{
    if (hasBeenInit) return;

    dataTransforms[Linear]
        .reset(new TransformLinear);
    
    dataTransforms[Logarithmic]
        .reset(new TransformLogarithmic);
    
    dataTransforms[Mel]
        .reset(new TransformMel);

    dataTransforms[Bark]
        .reset(new TransformBark);

    axisTransforms[Normal]
        .reset(new AxisTransformNormal);

    axisTransforms[Reverse]
        .reset(new AxisTransformReverse);
    
    hasBeenInit = true;
}

PlotTransform *PlotTransform::data(DataTransform type)
{
    initInstances();
    return dataTransforms[type].get();
}

PlotTransform *PlotTransform::axis(AxisTransform type)
{
    initInstances();
    return axisTransforms[type].get();
}

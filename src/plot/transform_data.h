#ifndef PLOT_TRANSFORM_DATA_H
#define PLOT_TRANSFORM_DATA_H

class TransformLinear : public PlotTransform
{
public:
    inline virtual double forward(double min, double max, double x) const
    {
        return (x - min) / (max - min);
    }

    inline virtual double backward(double min, double max, double x) const
    {
        return min + x * (max - min);
    }
};

class TransformLogarithmic : public PlotTransform
{
public:
    inline virtual double forward(double min, double max, double x) const
    {
        return std::log(x / min) / std::log(max / min);
    }

    inline virtual double backward(double min, double max, double x) const
    {
        return min * std::exp(x * std::log(max / min));
    }
};

class TransformMel : public PlotTransform
{
public:
    inline virtual double forward(double min, double max, double x) const
    {
        const double mMin = hz2mel(min);
        const double mMax = hz2mel(max);
        const double mx = hz2mel(x);

        return (mx - mMin) / (mMax - mMin);
    }

    inline virtual double backward(double min, double max, double x) const
    {
        const double mMin = hz2mel(min);
        const double mMax = hz2mel(max);
        const double y = mMin + x * (mMax - mMin);
        
        return mel2hz(y);
    }

private:
    inline static double hz2mel(double f) {
        return 1127.0 * std::log(1.0 + f / 700.0);
    }

    inline static double mel2hz(double m) {
        return 700.0 * (std::exp(m / 1127.0) - 1.0);
    }
};

class TransformBark : public PlotTransform
{
public:
    inline virtual double forward(double min, double max, double x) const
    {
        const double bMin = hz2bark(min);
        const double bMax = hz2bark(max);
        const double bx = hz2bark(x);

        return (bx - bMin) / (bMax - bMin);
    }

    inline virtual double backward(double min, double max, double x) const
    {
        const double bMin = hz2bark(min);
        const double bMax = hz2bark(max);
        const double y = bMin + x * (bMax - bMin);

        return bark2hz(y);
    }

private:
    inline static double hz2bark(double f) {
        return 6.0 * std::asinh(f / 600.0);
    }

    inline static double bark2hz(double b) {
        return 600.0 * std::sinh(b / 6.0);
    }
};

#endif // PLOT_TRANSFORM_DATA_H

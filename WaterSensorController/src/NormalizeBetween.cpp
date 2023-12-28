#include "NormalizeBetween.h"

double normalizeBetween(
    double val,
    double minVal,
    double maxVal,
    double newMin,
    double newMax)
{
    return newMin + ((val - minVal) * (newMax - newMin)) / (maxVal - minVal);
}

#include "EPSFBuilderParameters.h"

namespace pcl
{

EPSFBuilderMaxStars* TheEPSFBuilderMaxStarsParameter = nullptr;
EPSFBuilderStarMaxPeak* TheEPSFBuilderStarMaxPeakParameter = nullptr;
EPSFBuilderStarThreshold* TheEPSFBuilderStarThresholdParameter = nullptr;
EPSFBuilderStarFWHM* TheEPSFBuilderStarFWHMParameter = nullptr;
EPSFBuilderStarSize* TheEPSFBuilderStarSizeParameter = nullptr;
EPSFBuilderOversampling* TheEPSFBuilderOversamplingParameter = nullptr;
EPSFBuilderSmoothingKernel* TheEPSFBuilderSmoothingKernelParameter = nullptr;
EPSFBuilderMaxIterations* TheEPSFBuilderMaxIterationsParameter = nullptr;

// Maximum number of brightest stars for star detection

EPSFBuilderMaxStars::EPSFBuilderMaxStars(MetaProcess* P) : MetaInt32(P)
{
    TheEPSFBuilderMaxStarsParameter = this;
}

IsoString EPSFBuilderMaxStars::Id() const
{
    return "maxStars";
}

double EPSFBuilderMaxStars::MinimumValue() const
{
    return 10.0;
}

double EPSFBuilderMaxStars::MaximumValue() const
{
    return 500.0;
}

double EPSFBuilderMaxStars::DefaultValue() const
{
    return 100.0;
}

// Maximum peak value of a star that will be selected in star detection

EPSFBuilderStarMaxPeak::EPSFBuilderStarMaxPeak(MetaProcess* P) : MetaFloat(P)
{
    TheEPSFBuilderStarMaxPeakParameter = this;
}

IsoString EPSFBuilderStarMaxPeak::Id() const
{
    return "starMaxPeak";
}

int EPSFBuilderStarMaxPeak::Precision() const
{
    return 1;
}

double EPSFBuilderStarMaxPeak::MinimumValue() const
{
    return 0.1;
}

double EPSFBuilderStarMaxPeak::MaximumValue() const
{
    return 1.0;
}

double EPSFBuilderStarMaxPeak::DefaultValue() const
{
    return 0.7;
}

// Absolute threshold value above which to detect a star

EPSFBuilderStarThreshold::EPSFBuilderStarThreshold(MetaProcess* P) : MetaFloat(P)
{
    TheEPSFBuilderStarThresholdParameter = this;
}

IsoString EPSFBuilderStarThreshold::Id() const
{
    return "starThreshold";
}

int EPSFBuilderStarThreshold::Precision() const
{
    return 3;
}

double EPSFBuilderStarThreshold::MinimumValue() const
{
    return 0.0;
}

double EPSFBuilderStarThreshold::MaximumValue() const
{
    return 1.0;
}

double EPSFBuilderStarThreshold::DefaultValue() const
{
    return 0.01;
}

// FWHM (full-width half-maximum) of the Gaussian kernel in units of pixels

EPSFBuilderStarFWHM::EPSFBuilderStarFWHM(MetaProcess* P) : MetaFloat(P)
{
    TheEPSFBuilderStarFWHMParameter = this;
}

IsoString EPSFBuilderStarFWHM::Id() const
{
    return "starFWHM";
}

int EPSFBuilderStarFWHM::Precision() const
{
    return 1;
}

double EPSFBuilderStarFWHM::MinimumValue() const
{
    return 1.0;
}

double EPSFBuilderStarFWHM::MaximumValue() const
{
    return 10.0;
}

double EPSFBuilderStarFWHM::DefaultValue() const
{
    return 3.0;
}

// Star size in star extraction and of the generated ePSF in pixels along each axis

EPSFBuilderStarSize::EPSFBuilderStarSize(MetaProcess* P) : MetaInt8(P)
{
    TheEPSFBuilderStarSizeParameter = this;
}

IsoString EPSFBuilderStarSize::Id() const
{
    return "starSize";
}

double EPSFBuilderStarSize::MinimumValue() const
{
    return 5.0;
}

double EPSFBuilderStarSize::MaximumValue() const
{
    return 100.0;
}

double EPSFBuilderStarSize::DefaultValue() const
{
    return 45.0;
}

// Oversampling factor of drizzling when building the ePSF

EPSFBuilderOversampling::EPSFBuilderOversampling(MetaProcess* P) : MetaInt8(P)
{
    TheEPSFBuilderOversamplingParameter = this;
}

IsoString EPSFBuilderOversampling::Id() const
{
    return "oversampling";
}

double EPSFBuilderOversampling::MinimumValue() const
{
    return 1.0;
}

double EPSFBuilderOversampling::MaximumValue() const
{
    return 4.0;
}

double EPSFBuilderOversampling::DefaultValue() const
{
    return 2.0;
}

// The smoothing kernel to apply to the ePSF

EPSFBuilderSmoothingKernel::EPSFBuilderSmoothingKernel(MetaProcess* P) : MetaEnumeration(P)
{
    TheEPSFBuilderSmoothingKernelParameter = this;
}

IsoString EPSFBuilderSmoothingKernel::Id() const
{
    return "smoothingKernel";
}

size_type EPSFBuilderSmoothingKernel::NumberOfElements() const
{
    return NumberOfSmoothingKernel;
}

IsoString EPSFBuilderSmoothingKernel::ElementId(size_type i) const
{
    switch (i)
    {
    default:
    case Quadratic: return "Quadratic";
    case Quartic:   return "Quartic";
    }
}

int EPSFBuilderSmoothingKernel::ElementValue(size_type i) const
{
    return int(i);
}

size_type EPSFBuilderSmoothingKernel::DefaultValueIndex() const
{
    return Default;
}

// The maximum number of iterations to perform when building the ePSF

EPSFBuilderMaxIterations::EPSFBuilderMaxIterations(MetaProcess* P) : MetaInt8(P)
{
    TheEPSFBuilderMaxIterationsParameter = this;
}

IsoString EPSFBuilderMaxIterations::Id() const
{
    return "maxIterations";
}

double EPSFBuilderMaxIterations::MinimumValue() const
{
    return 1.0;
}

double EPSFBuilderMaxIterations::MaximumValue() const
{
    return 10.0;
}

double EPSFBuilderMaxIterations::DefaultValue() const
{
    return 5.0;
}

}	// namespace pcl
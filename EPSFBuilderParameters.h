#ifndef __EPSFBuilderParameters_h
#define __EPSFBuilderParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// Parameters for star detection

class EPSFBuilderMaxStars : public MetaInt32
{
public:
    EPSFBuilderMaxStars(MetaProcess*);

    IsoString Id() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderMaxStars* TheEPSFBuilderMaxStarsParameter;

class EPSFBuilderStarMaxPeak : public MetaFloat
{
public:
    EPSFBuilderStarMaxPeak(MetaProcess*);

    IsoString Id() const override;
    int Precision() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderStarMaxPeak* TheEPSFBuilderStarMaxPeakParameter;

class EPSFBuilderStarThreshold : public MetaFloat
{
public:
    EPSFBuilderStarThreshold(MetaProcess*);

    IsoString Id() const override;
    int Precision() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderStarThreshold* TheEPSFBuilderStarThresholdParameter;

class EPSFBuilderStarFWHM : public MetaFloat
{
public:
    EPSFBuilderStarFWHM(MetaProcess*);

    IsoString Id() const override;
    int Precision() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderStarFWHM* TheEPSFBuilderStarFWHMParameter;

// Parameters for building ePSF

class EPSFBuilderStarSize : public MetaInt8
{
public:
    EPSFBuilderStarSize(MetaProcess*);

    IsoString Id() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderStarSize* TheEPSFBuilderStarSizeParameter;

class EPSFBuilderOversampling : public MetaInt8
{
public:
    EPSFBuilderOversampling(MetaProcess*);

    IsoString Id() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderOversampling* TheEPSFBuilderOversamplingParameter;

class EPSFBuilderSmoothingKernel : public MetaEnumeration
{
public:

    enum {
        Quartic, Quadratic, NumberOfSmoothingKernel, Default = Quadratic
    };

    EPSFBuilderSmoothingKernel(MetaProcess*);

    IsoString Id() const override;
    size_type NumberOfElements() const override;
    IsoString ElementId(size_type) const override;
    int ElementValue(size_type) const override;
    size_type DefaultValueIndex() const override;
};

extern EPSFBuilderSmoothingKernel* TheEPSFBuilderSmoothingKernelParameter;

class EPSFBuilderMaxIterations : public MetaInt8
{
public:
    EPSFBuilderMaxIterations(MetaProcess*);

    IsoString Id() const override;
    double MinimumValue() const override;
    double MaximumValue() const override;
    double DefaultValue() const override;
};

extern EPSFBuilderMaxIterations* TheEPSFBuilderMaxIterationsParameter;

PCL_END_LOCAL

}	// namespace pcl

#endif	// __EPSFBuilderParameters_h
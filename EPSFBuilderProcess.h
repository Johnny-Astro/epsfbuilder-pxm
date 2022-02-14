#ifndef __EPSFBuilderProcess_h
#define __EPSFBuilderProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

class EPSFBuilderProcess : public MetaProcess
{
public:
    EPSFBuilderProcess();

    IsoString Id() const override;
    IsoString Category() const override;
    uint32 Version() const override;
    String Description() const override;
    IsoString IconImageSVG() const override;
    ProcessInterface* DefaultInterface() const override;
    ProcessImplementation* Create() const override;
    ProcessImplementation* Clone(const ProcessImplementation&) const override;
    bool NeedsValidation() const override;
    bool CanProcessCommandLines() const override;
};

PCL_BEGIN_LOCAL
extern EPSFBuilderProcess* TheEPSFBuilderProcess;
PCL_END_LOCAL

}	// namespace pcl

#endif	// __EPSFBuilderProcess_h
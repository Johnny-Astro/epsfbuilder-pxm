#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2022
#define MODULE_RELEASE_MONTH     2
#define MODULE_RELEASE_DAY       14

#include "EPSFBuilderModule.h"
#include "EPSFBuilderProcess.h"
#include "EPSFBuilderInterface.h"

namespace pcl
{

EPSFBuilderModule::EPSFBuilderModule()
{
}

const char* EPSFBuilderModule::Version() const
{
    return PCL_MODULE_VERSION(MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE);
}

IsoString EPSFBuilderModule::Name() const
{
    return "EPSFBuilder";
}

String EPSFBuilderModule::Description() const
{
    return "PixInsight EPSFBuilder Module";
}

String EPSFBuilderModule::Company() const
{
    return "N/A";
}

String EPSFBuilderModule::Author() const
{
    return "Johnny Qiu";
}

String EPSFBuilderModule::Copyright() const
{
    return "Copyright (c) 2022 Johnny Qiu";
}

String EPSFBuilderModule::TradeMarks() const
{
    return "JQ";
}

String EPSFBuilderModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
    return "epsfbuilder-pxm.so";
#endif
#ifdef __PCL_FREEBSD
    return "epsfbuilder-pxm.so";
#endif
#ifdef __PCL_MACOSX
    return "epsfbuilder-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
    return "epsfbuilder-pxm.dll";
#endif
}

void EPSFBuilderModule::GetReleaseDate(int& year, int& month, int& day) const
{
    year = MODULE_RELEASE_YEAR;
    month = MODULE_RELEASE_MONTH;
    day = MODULE_RELEASE_DAY;
}

}   // namespace pcl

PCL_MODULE_EXPORT int InstallPixInsightModule(int mode)
{
    new pcl::EPSFBuilderModule;

    if (mode == pcl::InstallMode::FullInstall) {
        new pcl::EPSFBuilderProcess;
        new pcl::EPSFBuilderInterface;
    }

    return 0;
}
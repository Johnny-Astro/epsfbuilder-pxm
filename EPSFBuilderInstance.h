#ifndef __EPSFBuilderInstance_h
#define __EPSFBuilderInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_enum

namespace pcl
{

class EPSFBuilderInstance : public ProcessImplementation
{
public:
    EPSFBuilderInstance(const MetaProcess*);
    EPSFBuilderInstance(const EPSFBuilderInstance&);

    void Assign(const ProcessImplementation&) override;
    bool IsHistoryUpdater(const View& v) const override;
    UndoFlags UndoMode(const View&) const override;
    bool CanExecuteOn(const View&, pcl::String& whyNot) const override;
    bool ExecuteOn(View&) override;
    void* LockParameter(const MetaParameter*, size_type tableRow) override;

private:
    String pythonDll;
    int maxStars;
    double starMaxPeak;
    double starThreshold;
    double starFWHM;
    int starSize;
    int oversampling;
    pcl_enum smoothingKernel;
    int maxIterations;

    static HMODULE m_hPythonDll;

    template<typename T>
    T loadPythonAPI(LPCSTR funcName)
    {
        T func = (T)GetProcAddress(m_hPythonDll, funcName);
        if (func == nullptr)
            throw Error("Failed to get function " + String(funcName) + " from Python DLL");
        return func;
    }

    friend class EPSFBuilderProcess;
    friend class EPSFBuilderInterface;
};

}	// namespace pcl

#endif	// __EPSFBuilderInstance_h
#ifndef __EPSFBuilderInterface_h
#define __EPSFBuilderInterface_h

#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "EPSFBuilderInstance.h"

namespace pcl {

class EPSFBuilderInterface : public ProcessInterface
{
public:
    EPSFBuilderInterface();
    virtual ~EPSFBuilderInterface();

    IsoString Id() const override;
    MetaProcess* Process() const override;
    IsoString IconImageSVG() const override;
    InterfaceFeatures Features() const override;
    void ApplyInstance() const override;
    void ResetInstance() override;
    bool Launch(const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/) override;
    ProcessImplementation* NewProcess() const override;
    bool ValidateProcess(const ProcessImplementation&, pcl::String& whyNot) const override;
    bool RequiresInstanceValidation() const override;
    bool ImportProcess(const ProcessImplementation&) override;

private:
    EPSFBuilderInstance instance;

    struct GUIData
    {
        GUIData(EPSFBuilderInterface&);

        VerticalSizer   Global_Sizer;

        SectionBar      Python_SectionBar;
        Control         Python_Control;
        HorizontalSizer Python_Sizer;
            Label           PythonDLL_Label;
            Edit              PythonDLL_Edit;
            ToolButton        PythonDLL_ToolButton;

        SectionBar      StarDetection_SectionBar;
        Control         StarDetection_Control;
        VerticalSizer   StarDetection_Sizer;
            NumericControl  MaxStars_NumericControl;
            NumericControl  StarMaxPeak_NumericControl;
            NumericControl  StarThreshold_NumericControl;
            NumericControl  StarFWHM_NumericControl;

        SectionBar      EPSFFitting_SectionBar;
        Control         EPSFFitting_Control;
        VerticalSizer   EPSFFitting_Sizer;
            NumericControl  StarSize_NumericControl;
            NumericControl  Oversampling_NumericControl;
            HorizontalSizer SmoothingKernel_Sizer;
                Label           SmoothingKernel_Label;
                ComboBox        SmoothingKernel_ComboBox;
            NumericControl  MaxIterations_NumericControl;
    };

    GUIData* GUI = nullptr;

    void UpdateControls();
    void __EditValueUpdated(NumericEdit& sender, double value);
    void __SmoothingKernel_ItemSelected(ComboBox& sender, int itemIndex);
    void __EditCompleted(Edit& sender);
    void __Click(Button& sender, bool checked);

    friend struct GUIData;
};

PCL_BEGIN_LOCAL
extern EPSFBuilderInterface* TheEPSFBuilderInterface;
PCL_END_LOCAL

}	// namespace pcl

#endif  // __EPSFBuilderInterface_h
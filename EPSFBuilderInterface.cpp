#include "EPSFBuilderInterface.h"
#include "EPSFBuilderParameters.h"
#include "EPSFBuilderProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/FileDialog.h>
#include <pcl/Settings.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

EPSFBuilderInterface* TheEPSFBuilderInterface = nullptr;

EPSFBuilderInterface::EPSFBuilderInterface()
	: instance(TheEPSFBuilderProcess)
{
	TheEPSFBuilderInterface = this;
}

EPSFBuilderInterface::~EPSFBuilderInterface()
{
	if (GUI != nullptr)
		delete GUI, GUI = nullptr;
}

IsoString EPSFBuilderInterface::Id() const
{
	return "EPSFBuilder";
}

MetaProcess* EPSFBuilderInterface::Process() const
{
	return TheEPSFBuilderProcess;
}

IsoString EPSFBuilderInterface::IconImageSVG() const
{
	return TheEPSFBuilderProcess->IconImageSVG();
}

InterfaceFeatures EPSFBuilderInterface::Features() const
{
	return InterfaceFeature::Default;
}

void EPSFBuilderInterface::ApplyInstance() const
{
	instance.LaunchOnCurrentView();
}

void EPSFBuilderInterface::ResetInstance()
{
	EPSFBuilderInstance defaultInstance(TheEPSFBuilderProcess);
	ImportProcess(defaultInstance);
}

bool EPSFBuilderInterface::Launch(const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/)
{
	if (GUI == nullptr)
	{
		GUI = new GUIData(*this);
		SetWindowTitle("ePSF Builder");
		Settings::Read("PythonDLL", instance.pythonDll);
		UpdateControls();
	}

	dynamic = false;
	return &P == TheEPSFBuilderProcess;
}

ProcessImplementation* EPSFBuilderInterface::NewProcess() const
{
	return new EPSFBuilderInstance(instance);
}

bool EPSFBuilderInterface::ValidateProcess(const ProcessImplementation& p, String& whyNot) const
{
	if (dynamic_cast<const EPSFBuilderInstance*>(&p) != nullptr)
		return true;
	whyNot = "Not a EPSFBuilder instance.";
	return false;
}

bool EPSFBuilderInterface::RequiresInstanceValidation() const
{
	return true;
}

bool EPSFBuilderInterface::ImportProcess(const ProcessImplementation& p)
{
	instance.Assign(p);
	UpdateControls();
	return true;
}

void EPSFBuilderInterface::UpdateControls()
{
	GUI->PythonDLL_Edit.SetText(instance.pythonDll);
	Settings::Write("PythonDLL", instance.pythonDll);
	GUI->MaxStars_NumericControl.SetValue(instance.maxStars);
	GUI->StarMaxPeak_NumericControl.SetValue(instance.starMaxPeak);
	GUI->StarThreshold_NumericControl.SetValue(instance.starThreshold);
	GUI->StarFWHM_NumericControl.SetValue(instance.starFWHM);
	GUI->StarSize_NumericControl.SetValue(instance.starSize);
	GUI->Oversampling_NumericControl.SetValue(instance.oversampling);
	GUI->SmoothingKernel_ComboBox.SetCurrentItem(instance.smoothingKernel);
	GUI->MaxIterations_NumericControl.SetValue(instance.maxIterations);
}

void EPSFBuilderInterface::__EditValueUpdated(NumericEdit& sender, double value)
{
	if (sender == GUI->MaxStars_NumericControl)
		instance.maxStars = value;
	else if (sender == GUI->StarMaxPeak_NumericControl)
		instance.starMaxPeak = value;
	else if (sender == GUI->StarThreshold_NumericControl)
		instance.starThreshold = value;
	else if (sender == GUI->StarFWHM_NumericControl)
		instance.starFWHM = value;
	else if (sender == GUI->StarSize_NumericControl)
		instance.starSize = value;
	else if (sender == GUI->Oversampling_NumericControl)
		instance.oversampling = value;
	else if (sender == GUI->MaxIterations_NumericControl)
		instance.maxIterations = value;
}

void EPSFBuilderInterface::__SmoothingKernel_ItemSelected(ComboBox& /*sender*/, int itemIndex)
{
	instance.smoothingKernel = itemIndex;
}

void EPSFBuilderInterface::__EditCompleted(Edit& sender)
{
	try
	{
		String filePath = sender.Text().Trimmed();
		if (sender == GUI->PythonDLL_Edit)
			instance.pythonDll = filePath;
		UpdateControls();
	}
	ERROR_CLEANUP(
		sender.SelectAll();
		sender.Focus()
	)
}

void EPSFBuilderInterface::__Click(Button& sender, bool checked)
{
	if (sender == GUI->PythonDLL_ToolButton)
	{
		OpenFileDialog d;
		d.SetCaption("ePSF Builder: Select Python DLL");
		d.AddFilter(FileFilter("DLL Files", ".DLL"));
		d.AddFilter(FileFilter("Any Files", "*"));
		d.DisableMultipleSelections();
		if (d.Execute())
		{
			instance.pythonDll = d.FileName();
			UpdateControls();
		}
	}
}

EPSFBuilderInterface::GUIData::GUIData(EPSFBuilderInterface& w)
{
	pcl::Font fnt = w.Font();
	int labelWidth1 = fnt.Width(String("Maximum Iterations:") + 'T');
	int editWidth1 = fnt.Width(String('0', 10));

	Python_SectionBar.SetTitle("Python");
	Python_SectionBar.SetSection(Python_Control);

	const char* pythonDllToolTip = "<p>Path to Python DLL. Require to restart PixInsight if switching DLL.</p>";

	PythonDLL_Label.SetText("Python DLL:");
	PythonDLL_Label.SetFixedWidth(labelWidth1);
	PythonDLL_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	PythonDLL_Label.SetToolTip(pythonDllToolTip);

	PythonDLL_Edit.SetToolTip(pythonDllToolTip);
	PythonDLL_Edit.SetMinWidth(fnt.Width(String('0', 45)));
	PythonDLL_Edit.OnEditCompleted((Edit::edit_event_handler) & EPSFBuilderInterface::__EditCompleted, w);

	PythonDLL_ToolButton.SetIcon(w.ScaledResource(":/browser/select-file.png"));
	PythonDLL_ToolButton.SetScaledFixedSize(20, 20);
	PythonDLL_ToolButton.SetToolTip("<p>Select Python DLL</p>");
	PythonDLL_ToolButton.OnClick((Button::click_event_handler) & EPSFBuilderInterface::__Click, w);

	Python_Sizer.SetSpacing(4);
	Python_Sizer.Add(PythonDLL_Label);
	Python_Sizer.Add(PythonDLL_Edit, 100);
	Python_Sizer.Add(PythonDLL_ToolButton);
	Python_Sizer.AddStretch();

	Python_Control.SetSizer(Python_Sizer);

	StarDetection_SectionBar.SetTitle("Star Detection");
	StarDetection_SectionBar.SetSection(StarDetection_Control);

	MaxStars_NumericControl.label.SetText("Maximum stars:");
	MaxStars_NumericControl.label.SetFixedWidth(labelWidth1);
	MaxStars_NumericControl.slider.SetRange(0, 500);
	MaxStars_NumericControl.slider.SetScaledMinWidth(300);
	MaxStars_NumericControl.SetInteger();
	MaxStars_NumericControl.SetRange(TheEPSFBuilderMaxStarsParameter->MinimumValue(), TheEPSFBuilderMaxStarsParameter->MaximumValue());
	MaxStars_NumericControl.edit.SetFixedWidth(editWidth1);
	MaxStars_NumericControl.SetToolTip("<p>Maximum number of brightest stars that will be detected for ePSF fitting.</p>");
	MaxStars_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	StarMaxPeak_NumericControl.label.SetText("Maximum peak:");
	StarMaxPeak_NumericControl.label.SetFixedWidth(labelWidth1);
	StarMaxPeak_NumericControl.slider.SetRange(0, 500);
	StarMaxPeak_NumericControl.slider.SetScaledMinWidth(300);
	StarMaxPeak_NumericControl.SetReal();
	StarMaxPeak_NumericControl.SetRange(TheEPSFBuilderStarMaxPeakParameter->MinimumValue(), TheEPSFBuilderStarMaxPeakParameter->MaximumValue());
	StarMaxPeak_NumericControl.SetPrecision(TheEPSFBuilderStarMaxPeakParameter->Precision());
	StarMaxPeak_NumericControl.edit.SetFixedWidth(editWidth1);
	StarMaxPeak_NumericControl.SetToolTip("<p>Maximum peak value of a star that will be selected in star detection. This can be used to prevent the saturated stars from being selected.</p>");
	StarMaxPeak_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	StarThreshold_NumericControl.label.SetText("Threshold:");
	StarThreshold_NumericControl.label.SetFixedWidth(labelWidth1);
	StarThreshold_NumericControl.slider.SetRange(0, 500);
	StarThreshold_NumericControl.slider.SetScaledMinWidth(300);
	StarThreshold_NumericControl.SetReal();
	StarThreshold_NumericControl.SetRange(TheEPSFBuilderStarThresholdParameter->MinimumValue(), TheEPSFBuilderStarThresholdParameter->MaximumValue());
	StarThreshold_NumericControl.SetPrecision(TheEPSFBuilderStarThresholdParameter->Precision());
	StarThreshold_NumericControl.edit.SetFixedWidth(editWidth1);
	StarThreshold_NumericControl.SetToolTip("<p>Absolute threshold value above which to detect a star.</p>");
	StarThreshold_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	StarFWHM_NumericControl.label.SetText("FWHM:");
	StarFWHM_NumericControl.label.SetFixedWidth(labelWidth1);
	StarFWHM_NumericControl.slider.SetRange(0, 500);
	StarFWHM_NumericControl.slider.SetScaledMinWidth(300);
	StarFWHM_NumericControl.SetReal();
	StarFWHM_NumericControl.SetRange(TheEPSFBuilderStarFWHMParameter->MinimumValue(), TheEPSFBuilderStarFWHMParameter->MaximumValue());
	StarFWHM_NumericControl.SetPrecision(TheEPSFBuilderStarFWHMParameter->Precision());
	StarFWHM_NumericControl.edit.SetFixedWidth(editWidth1);
	StarFWHM_NumericControl.SetToolTip("<p>FWHM (full-width half-maximum) of the Gaussian kernel in units of pixels.</p>");
	StarFWHM_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	StarDetection_Sizer.SetSpacing(4);
	StarDetection_Sizer.Add(MaxStars_NumericControl);
	StarDetection_Sizer.Add(StarMaxPeak_NumericControl);
	StarDetection_Sizer.Add(StarThreshold_NumericControl);
	StarDetection_Sizer.Add(StarFWHM_NumericControl);
	StarDetection_Sizer.AddStretch();

	StarDetection_Control.SetSizer(StarDetection_Sizer);

	EPSFFitting_SectionBar.SetTitle("ePSF Fitting");
	EPSFFitting_SectionBar.SetSection(EPSFFitting_Control);

	StarSize_NumericControl.label.SetText("Star size:");
	StarSize_NumericControl.label.SetFixedWidth(labelWidth1);
	StarSize_NumericControl.slider.SetRange(0, 500);
	StarSize_NumericControl.slider.SetScaledMinWidth(300);
	StarSize_NumericControl.SetInteger();
	StarSize_NumericControl.SetRange(TheEPSFBuilderStarSizeParameter->MinimumValue(), TheEPSFBuilderStarSizeParameter->MaximumValue());
	StarSize_NumericControl.edit.SetFixedWidth(editWidth1);
	StarSize_NumericControl.SetToolTip("<p>Star size in star extraction and of the generated ePSF in pixels along each axis.</p>");
	StarSize_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	Oversampling_NumericControl.label.SetText("Oversampling:");
	Oversampling_NumericControl.label.SetFixedWidth(labelWidth1);
	Oversampling_NumericControl.slider.SetRange(0, 500);
	Oversampling_NumericControl.slider.SetScaledMinWidth(300);
	Oversampling_NumericControl.SetInteger();
	Oversampling_NumericControl.SetRange(TheEPSFBuilderOversamplingParameter->MinimumValue(), TheEPSFBuilderOversamplingParameter->MaximumValue());
	Oversampling_NumericControl.edit.SetFixedWidth(editWidth1);
	Oversampling_NumericControl.SetToolTip("<p>Oversampling factor of drizzling when building the ePSF.</p>");
	Oversampling_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	SmoothingKernel_Label.SetText("Smoothing Kernel:");
	SmoothingKernel_Label.SetFixedWidth(labelWidth1);
	SmoothingKernel_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	SmoothingKernel_ComboBox.AddItem("Quadratic");
	SmoothingKernel_ComboBox.AddItem("Quartic");
	SmoothingKernel_ComboBox.SetToolTip("<p>The smoothing kernel to apply to the ePSF.</p>");
	SmoothingKernel_ComboBox.OnItemSelected((ComboBox::item_event_handler) & EPSFBuilderInterface::__SmoothingKernel_ItemSelected, w);
	SmoothingKernel_Sizer.SetSpacing(4);
	SmoothingKernel_Sizer.Add(SmoothingKernel_Label);
	SmoothingKernel_Sizer.Add(SmoothingKernel_ComboBox);
	SmoothingKernel_Sizer.AddStretch();

	MaxIterations_NumericControl.label.SetText("Maximum Iterations:");
	MaxIterations_NumericControl.label.SetFixedWidth(labelWidth1);
	MaxIterations_NumericControl.slider.SetRange(0, 500);
	MaxIterations_NumericControl.slider.SetScaledMinWidth(300);
	MaxIterations_NumericControl.SetInteger();
	MaxIterations_NumericControl.SetRange(TheEPSFBuilderMaxIterationsParameter->MinimumValue(), TheEPSFBuilderMaxIterationsParameter->MaximumValue());
	MaxIterations_NumericControl.edit.SetFixedWidth(editWidth1);
	MaxIterations_NumericControl.SetToolTip("<p>The maximum number of iterations to perform when building the ePSF.</p>");
	MaxIterations_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & EPSFBuilderInterface::__EditValueUpdated, w);

	EPSFFitting_Sizer.AddSpacing(4);
	EPSFFitting_Sizer.Add(StarSize_NumericControl);
	EPSFFitting_Sizer.Add(Oversampling_NumericControl);
	EPSFFitting_Sizer.Add(SmoothingKernel_Sizer);
	EPSFFitting_Sizer.Add(MaxIterations_NumericControl);
	EPSFFitting_Sizer.AddStretch();

	EPSFFitting_Control.SetSizer(EPSFFitting_Sizer);

	Global_Sizer.SetMargin(8);
	Global_Sizer.SetSpacing(4);
	Global_Sizer.Add(Python_SectionBar);
	Global_Sizer.Add(Python_Control);
	Global_Sizer.Add(StarDetection_SectionBar);
	Global_Sizer.Add(StarDetection_Control);
	Global_Sizer.Add(EPSFFitting_SectionBar);
	Global_Sizer.Add(EPSFFitting_Control);

	w.SetSizer(Global_Sizer);

	w.EnsureLayoutUpdated();
	w.AdjustToContents();
	w.SetFixedSize();
}

}	// namespace pcl
#include <Tchar.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <pcl/AtrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/DisplayFunction.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/IntegerResample.h>
#include <pcl/Settings.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

#include "EPSFBuilderInstance.h"
#include "EPSFBuilderParameters.h"

namespace pcl
{

HMODULE EPSFBuilderInstance::m_hPythonDll = NULL;

EPSFBuilderInstance::EPSFBuilderInstance(const MetaProcess* m)
    : ProcessImplementation(m)
    , maxStars(TheEPSFBuilderMaxStarsParameter->DefaultValue())
    , starMaxPeak(TheEPSFBuilderStarMaxPeakParameter->DefaultValue())
    , starThreshold(TheEPSFBuilderStarThresholdParameter->DefaultValue())
    , starFWHM(TheEPSFBuilderStarFWHMParameter->DefaultValue())
    , starSize(TheEPSFBuilderStarSizeParameter->DefaultValue())
    , oversampling(TheEPSFBuilderOversamplingParameter->DefaultValue())
    , smoothingKernel(static_cast<pcl_enum>(TheEPSFBuilderSmoothingKernelParameter->DefaultValueIndex()))
    , maxIterations(TheEPSFBuilderMaxIterationsParameter->DefaultValue())
{
}

EPSFBuilderInstance::EPSFBuilderInstance(const EPSFBuilderInstance& x)
    : ProcessImplementation(x)
{
    Assign(x);
}

void EPSFBuilderInstance::Assign(const ProcessImplementation& p)
{
    const EPSFBuilderInstance* x = dynamic_cast<const EPSFBuilderInstance*>(&p);
    if (x != nullptr) {
        pythonDll = x->pythonDll;
        maxStars = x->maxStars;
        starMaxPeak = x->starMaxPeak;
        starThreshold = x->starThreshold;
        starFWHM = x->starFWHM;
        starSize = x->starSize;
        oversampling = x->oversampling;
        smoothingKernel = x->smoothingKernel;
        maxIterations = x->maxIterations;
    }
}

bool EPSFBuilderInstance::IsHistoryUpdater(const View& view) const
{
    return false;
}

UndoFlags EPSFBuilderInstance::UndoMode(const View&) const
{
    return UndoFlag::PixelData;
}

bool EPSFBuilderInstance::CanExecuteOn(const View& view, pcl::String& whyNot) const
{
    if (view.Image().IsComplexSample())
    {
        whyNot = "ePSF Builder cannot be executed on complex images.";
        return false;
    }
    else if (!view.Image().IsFloatSample())
    {
        whyNot = "ePSF Builder can only be executed on float images.";
        return false;
    }
    else if (view.Image().NumberOfChannels() != 1)
    {
        whyNot = "ePSF Builder can only be executed on single channel images.";
        return false;
    }

    return true;
}

bool EPSFBuilderInstance::ExecuteOn(View& view)
{
    AutoViewLock lock(view);

    StandardStatus status;
    Console console;

    console.EnableAbort();

    ImageVariant image = view.Image();

    if (image.IsComplexSample() || !view.Image().IsFloatSample() || (view.Image().NumberOfChannels() != 1))
        return false;

    image.SetStatusCallback(&status);

    StringList swapDirs = ImageWindow::SwapDirectories();
    if (swapDirs.Length() < 1)
        throw Error("Swap storage directories not set");

    // Step 0: prepare Python environment
    if (!m_hPythonDll)
    {
        m_hPythonDll = LoadLibrary((LPCWSTR)pythonDll.c_str());
        if (!m_hPythonDll)
            throw Error("Failed to load Python DLL: " + pythonDll);
    }

    typedef void(__cdecl* f_Py_Initialize)();
    f_Py_Initialize Py_Initialize = loadPythonAPI<f_Py_Initialize>("Py_Initialize");
    typedef int(__cdecl* f_Py_IsInitialized)();
    f_Py_IsInitialized Py_IsInitialized = loadPythonAPI<f_Py_IsInitialized>("Py_IsInitialized");
    typedef int(__cdecl* f_PyRun_SimpleString)(const char*);
    f_PyRun_SimpleString PyRun_SimpleString = loadPythonAPI<f_PyRun_SimpleString>("PyRun_SimpleString");

    String why = "";
    if (!Py_IsInitialized())
    {
        Py_Initialize();
        if (!Py_IsInitialized())
            throw Error("Failed to initialize Python");
    }

#define RUN_PYTHON(x)                                                   \
    {                                                                   \
        if (PyRun_SimpleString(x) != 0)                                 \
            throw Error("Error running python script: " + String(x));   \
    }

#define LOG_PYTHON_VAR(x)                                       \
    {                                                           \
        RUN_PYTHON("logFp = open('E:/log.txt', 'a')");          \
        RUN_PYTHON("logFp.write(str(" ##x ") + '\\n')");        \
        RUN_PYTHON("logFp.close()");                            \
    }

    RUN_PYTHON("from astropy.io import fits");
    RUN_PYTHON("from astropy.nddata import NDData");
    RUN_PYTHON("from astropy.table import Table");
    RUN_PYTHON("from photutils.detection import DAOStarFinder");
    RUN_PYTHON("from photutils.psf import EPSFBuilder");
    RUN_PYTHON("from photutils.psf import EPSFStars");
    RUN_PYTHON("from photutils.psf import extract_stars");
    RUN_PYTHON("import numpy as np");

    // Step 1: remove local background
    ImageVariant starImage;
    starImage.CopyImage(image);
    starImage.EnsureUniqueImage();
    starImage.SetStatusCallback(nullptr);
    image.Status().Initialize("Removing background", 1);
    static const float B3S_hv[] = { 0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f };
    int layers = int(pcl::Log2<double>(starSize) + 2.5);
    StarletTransform mt(SeparableFilter(B3S_hv, B3S_hv, 5), layers);
    mt << starImage;
    mt.DisableLayer(layers);
    mt >> starImage;
    starImage.Truncate(-0.001, 1.0);
    starImage.Normalize();
    image.Status() += 1;
    image.Status().Complete();

    // Step 2: write to temporary FITS file
    String tmpImgFilename = swapDirs[0] + "/tmp.fits";
    FileFormatInstance tmpFits(FileFormat(".fits", false, true));
    if (!tmpFits.Create(tmpImgFilename, "verbosity 0"))
        throw Error("Unable to write to swap storage");

    ImageOptions imgOpt;
    imgOpt.bitsPerSample = starImage.BitsPerSample();
    imgOpt.ieeefpSampleFormat = starImage.IsFloatSample();
    tmpFits.SetOptions(imgOpt);
    if (!tmpFits.WriteImage(starImage))
        throw Error("Unable to write to swap storage");
    tmpFits.Close();

    // Step 3: read FITS in Python
    char cmd[256];
    sprintf_s(cmd, sizeof(cmd), "hdul = fits.open('%s', memmap = False, lazy_load_hdus = False)", tmpImgFilename.ToUTF8().c_str());
    RUN_PYTHON(cmd);
    RUN_PYTHON("data = hdul[0].data");
    RUN_PYTHON("hdul.close()");

    // Step 4: star detection
    image.Status().Initialize("Running star detection", 1);
    sprintf_s(cmd, sizeof(cmd), "daofind = DAOStarFinder(fwhm = %lf, threshold = %lf, peakmax = %lf, brightest = %d)", starFWHM, starThreshold, starMaxPeak, maxStars * 3);
    RUN_PYTHON(cmd);
    RUN_PYTHON("sources = daofind(data)");
    RUN_PYTHON("starpos = np.transpose((sources['xcentroid'], sources['ycentroid']))");
    image.Status() += 1;
    image.Status().Complete();

    // Step 5: star extraction
    image.Status().Initialize("Extracting stars", 3);
    RUN_PYTHON("stars_tbl = Table()");
    RUN_PYTHON("stars_tbl['x'] = starpos[:, 0]");
    RUN_PYTHON("stars_tbl['y'] = starpos[:, 1]");
    RUN_PYTHON("nddata = NDData(data = data)");
    sprintf_s(cmd, sizeof(cmd), "stars = extract_stars(nddata, stars_tbl, size = %d)", (int)(starSize * 1.5));
    RUN_PYTHON(cmd);
    image.Status() += 1;

    // Step 6: filter stars
    RUN_PYTHON("good_stars = []");
    sprintf_s(cmd, sizeof(cmd), "daofind = DAOStarFinder(fwhm = %lf, threshold = %lf, brightest = 2)", starFWHM, starThreshold);
    RUN_PYTHON(cmd);
    RUN_PYTHON("for i in range(stars.n_stars):\n"
               "    star = stars.all_stars[i]\n"
               "    sources = daofind(star.data)\n"
               "    if not (sources is None) and (sources['id'].size == 1):\n"
               "        good_stars.append(star)\n");
    RUN_PYTHON("stars = EPSFStars(good_stars)");
    image.Status() += 1;

    sprintf_s(cmd, sizeof(cmd), "n_stars = min(stars.n_stars, %d)", maxStars);
    RUN_PYTHON(cmd);
    sprintf_s(cmd, sizeof(cmd), "starPath = '%s'", swapDirs[0].ToUTF8().c_str());
    RUN_PYTHON(cmd);
    RUN_PYTHON("fp = open(starPath + '/stars.txt', 'w')");
    RUN_PYTHON("for i in range(n_stars):\n"
               "    star = stars.all_stars[i]\n"
               "    hdu = fits.PrimaryHDU(star.data)\n"
               "    hdu.writeto(starPath + '/star' + str(i) + '.fits', overwrite = True)\n"
               "    fp.write(str(star.origin[0]) + ' ' + str(star.origin[1]) + ' ' + str(star.center[0]) + ' ' + str(star.center[1]) + ' ' + str(star.flux) + '\\n')");
    RUN_PYTHON("fp.close()");
    image.Status() += 1;
    image.Status().Complete();

    // Step 7: generate ePSF
    image.Status().Initialize("Building ePSF", 1);
    const char* smoothingKernelName[] = {
        "quartic",
        "quadratic",
    };
    sprintf_s(cmd, sizeof(cmd), "epsf_builder = EPSFBuilder(oversampling = %d, maxiters = %d, smoothing_kernel = '%s')", oversampling, maxIterations, smoothingKernelName[smoothingKernel]);
    RUN_PYTHON(cmd);
    RUN_PYTHON("epsf, fitted_stars = epsf_builder(stars)");
    image.Status() += 1;
    image.Status().Complete();

    // Step 8: write ePSF to FITS
    RUN_PYTHON("hdu = fits.PrimaryHDU(epsf.data)");
    String ePSFFilename = swapDirs[0] + "/epsf.fits";
    sprintf_s(cmd, sizeof(cmd), "hdu.writeto('%s', overwrite = True)", ePSFFilename.ToUTF8().c_str());
    RUN_PYTHON(cmd);

    // Step 9: read ePSF and stars
    std::ifstream infile((swapDirs[0] + "/stars.txt").ToUTF8().c_str());
    if (!infile.is_open())
        throw Error("Cannot get star information");
    std::string line;
    struct Star
    {
        double origin[2];
        double center[2];
        double flux;
        ImageVariant image;
    };
    std::vector<struct Star> stars;
    FileFormatInstance starFits(FileFormat(".fits", true, false));
    ImageDescriptionArray ida;
    int starCount = 0;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        Star star;
        if (!(iss >> star.origin[0] >> star.origin[1] >> star.center[0] >> star.center[1] >> star.flux))
            throw Error("Error getting star information");
        String starFilename = swapDirs[0] + "/star" + String(starCount) + ".fits";
        star.image.CreateImageAs(image);
        if (!starFits.Open(ida, starFilename, "verbosity 0") || !starFits.ReadImage(star.image))
        {
            infile.close();
            throw Error("Error getting image of extracted stars");
        }
        starFits.Close();
        int x0 = (star.image.Width() - starSize) / 2;
        int y0 = (star.image.Height() - starSize) / 2;
        star.image.CropTo(x0, y0, x0 + starSize, y0 + starSize);
        stars.push_back(star);
        starCount++;
    }
    infile.close();

    ImageVariant epsfImage;
    epsfImage.CreateImageAs(image);
    String epsfFilename = swapDirs[0] + "/epsf.fits";
    if (!starFits.Open(ida, epsfFilename, "verbosity 0") || !starFits.ReadImage(epsfImage))
        throw Error("Error getting image of ePSF");
    starFits.Close();

    // Crop, downscale and normalize
    int sz = starSize * oversampling;
    int x0 = (epsfImage.Width() - sz) / 2;
    int y0 = (epsfImage.Height() - sz) / 2;
    epsfImage.CropTo(x0, y0, x0 + sz, y0 + sz);
    IntegerResample ir(-oversampling);
    ir >> epsfImage;
    epsfImage.Subtract(epsfImage.MinimumSampleValue());
    epsfImage.Divide(epsfImage.MaximumSampleValue());

    // Create window for star detection
    ImageVariant starDetImage;
    starDetImage.CopyImage(image);
    starDetImage.EnsureUniqueImage();
    starDetImage.SetStatusCallback(nullptr);
    for (int i = 0; i < starCount; i++)
    {
        // Draw detection box
        int cx = stars[i].center[0];
        int cy = stars[i].center[1];
        int x0 = cx - starSize / 2;
        int y0 = cy - starSize / 2;
        for (int x = x0; x < x0 + starSize; x++)
        {
            if (starDetImage.BitsPerSample() == 32)
            {
                (static_cast<Image&>(*starDetImage))(x, y0) = 1;
                (static_cast<Image&>(*starDetImage))(x, y0 + starSize) = 1;
            }
            else if (starDetImage.BitsPerSample() == 64)
            {
                (static_cast<DImage&>(*starDetImage))(x, y0) = 1;
                (static_cast<DImage&>(*starDetImage))(x, y0 + starSize) = 1;
            }
        }
        for (int y = y0; y < y0 + starSize; y++)
        {
            if (starDetImage.BitsPerSample() == 32)
            {
                (static_cast<Image&>(*starDetImage))(x0, y) = 1;
                (static_cast<Image&>(*starDetImage))(x0 + starSize, y) = 1;
            }
            else if (starDetImage.BitsPerSample() == 64)
            {
                (static_cast<DImage&>(*starDetImage))(x0, y) = 1;
                (static_cast<DImage&>(*starDetImage))(x0 + starSize, y) = 1;
            }
        }
    }
    IsoString id = view.FullId() + "_star_detection";
    ImageWindow starDetWindow = ImageWindow(starDetImage.Width(), starDetImage.Height(), starDetImage.NumberOfChannels(), starDetImage.BitsPerSample(), starDetImage.IsFloatSample(), starDetImage.IsColor(), true, id);
    if (starDetWindow.IsNull())
        throw Error("Unable to create image window: " + id);
    starDetWindow.MainView().Lock();
    starDetWindow.MainView().Image().CopyImage(starDetImage);
    starDetWindow.MainView().Unlock();
    Vector center(1);
    Vector sigma(1);
    sigma[0] = 1.4826 * starDetImage.MAD(center[0] = starDetImage.Median());
    DisplayFunction DF;
    DF.SetLinkedRGB();
    DF.ComputeAutoStretch(sigma, center);
    starDetWindow.MainView().SetScreenTransferFunctions(DF.HistogramTransformations());
    starDetWindow.Show();

    // Create window for extracted stars
    int ncols = pcl::Sqrt(starCount);
    if (ncols < 2)
        ncols = 2;
    int nrows = (starCount + ncols - 1) / ncols;
    ImageVariant starListImage;
    starListImage.CreateImageAs(image);
    starListImage.SetStatusCallback(nullptr);
    starListImage.AllocateImage(ncols * starSize, nrows * starSize, image.NumberOfChannels(), image.ColorSpace());
    starListImage.Fill(0.0);
    for (int i = 0; i < starCount; i++)
    {
        int r = i / ncols;
        int c = i % ncols;
        int x0 = c * starSize;
        int y0 = r * starSize;
        for (int y = 0; y < starSize; y++)
            for (int x = 0; x < starSize; x++)
                if (starListImage.BitsPerSample() == 32)
                    (static_cast<Image&>(*starListImage))(x0 + x, y0 + y) = stars[i].image(x, y);
                else if (starListImage.BitsPerSample() == 64)
                    (static_cast<DImage&>(*starListImage))(x0 + x, y0 + y) = stars[i].image(x, y);
    }
  
    id = view.FullId() + "_extracted_stars";
    ImageWindow starListWindow = ImageWindow(starListImage.Width(), starListImage.Height(), starListImage.NumberOfChannels(), starListImage.BitsPerSample(), starListImage.IsFloatSample(), starListImage.IsColor(), true, id);
    if (starListWindow.IsNull())
        throw Error("Unable to create image window: " + id);
    starListWindow.MainView().Lock();
    starListWindow.MainView().Image().CopyImage(starListImage);
    starListWindow.MainView().Unlock();
    sigma[0] = 1.4826 * starListImage.MAD(center[0] = starListImage.Median());
    DF.SetLinkedRGB();
    DF.ComputeAutoStretch(sigma, center);
    starListWindow.MainView().SetScreenTransferFunctions(DF.HistogramTransformations());
    starListWindow.Show();

    // Create window for results
    id = view.FullId() + "_ePSF";
    ImageWindow ePSFWindow = ImageWindow(epsfImage.Width(), epsfImage.Height(), epsfImage.NumberOfChannels(), epsfImage.BitsPerSample(), epsfImage.IsFloatSample(), epsfImage.IsColor(), true, id);
    if (ePSFWindow.IsNull())
        throw Error("Unable to create image window: " + id);
    ePSFWindow.MainView().Lock();
    ePSFWindow.MainView().Image().CopyImage(epsfImage);
    ePSFWindow.MainView().Unlock();
    ePSFWindow.Show();

    return true;
}

void* EPSFBuilderInstance::LockParameter(const MetaParameter* p, size_type /*tableRow*/)
{
    if (p == TheEPSFBuilderMaxStarsParameter)
        return &maxStars;
    else if (p == TheEPSFBuilderStarMaxPeakParameter)
        return &starMaxPeak;
    else if (p == TheEPSFBuilderStarThresholdParameter)
        return &starThreshold;
    else if (p == TheEPSFBuilderStarFWHMParameter)
        return &starFWHM;
    else if (p == TheEPSFBuilderStarSizeParameter)
        return &starSize;
    else if (p == TheEPSFBuilderOversamplingParameter)
        return &oversampling;
    else if (p == TheEPSFBuilderSmoothingKernelParameter)
        return &smoothingKernel;
    else if (p == TheEPSFBuilderMaxIterationsParameter)
        return &maxIterations;
    return nullptr;
}

}	// namespace pcl
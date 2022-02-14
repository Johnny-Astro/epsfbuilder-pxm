#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

#include "EPSFBuilderProcess.h"
#include "EPSFBuilderParameters.h"
#include "EPSFBuilderInstance.h"
#include "EPSFBuilderInterface.h"

namespace pcl
{

EPSFBuilderProcess* TheEPSFBuilderProcess = nullptr;

EPSFBuilderProcess::EPSFBuilderProcess()
{
    TheEPSFBuilderProcess = this;

    // Instantiate process parameters
    new EPSFBuilderMaxStars(this);
    new EPSFBuilderStarMaxPeak(this);
    new EPSFBuilderStarThreshold(this);
    new EPSFBuilderStarFWHM(this);
    new EPSFBuilderStarSize(this);
    new EPSFBuilderOversampling(this);
    new EPSFBuilderSmoothingKernel(this);
    new EPSFBuilderMaxIterations(this);
}

IsoString EPSFBuilderProcess::Id() const
{
    return "EPSFBuilder";
}

IsoString EPSFBuilderProcess::Category() const
{
    return "Image";
}

// ----------------------------------------------------------------------------

uint32 EPSFBuilderProcess::Version() const
{
    return 0x100;
}

// ----------------------------------------------------------------------------

String EPSFBuilderProcess::Description() const
{
    return "Build the ePSF (effective PSF) from an image";
}

// ----------------------------------------------------------------------------

IsoString EPSFBuilderProcess::IconImageSVG() const
{
    return "<svg"
           "   xmlns:dc=\"http://purl.org/dc/elements/1.1/\""
           "   xmlns:cc=\"http://creativecommons.org/ns#\""
           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\""
           "   xmlns:svg=\"http://www.w3.org/2000/svg\""
           "   xmlns=\"http://www.w3.org/2000/svg\""
           "   xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\""
           "   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\""
           "   width=\"197.55556mm\""
           "   height=\"197.55556mm\""
           "   viewBox=\"0 0 700.00001 700\""
           "   id=\"svg2\""
           "   version=\"1.1\""
           "   inkscape:version=\"0.91 r13725\""
           "   sodipodi:docname=\"Star.svg\">"
           "  <defs"
           "     id=\"defs4\" />"
           "  <sodipodi:namedview"
           "     id=\"base\""
           "     pagecolor=\"#ffffff\""
           "     bordercolor=\"#666666\""
           "     borderopacity=\"1.0\""
           "     inkscape:pageopacity=\"0.0\""
           "     inkscape:pageshadow=\"2\""
           "     inkscape:zoom=\"0.95714285\""
           "     inkscape:cx=\"350\""
           "     inkscape:cy=\"350\""
           "     inkscape:document-units=\"px\""
           "     inkscape:current-layer=\"layer1\""
           "     showgrid=\"false\""
           "     fit-margin-top=\"0\""
           "     fit-margin-left=\"0\""
           "     fit-margin-right=\"0\""
           "     fit-margin-bottom=\"0\""
           "     inkscape:window-width=\"1600\""
           "     inkscape:window-height=\"837\""
           "     inkscape:window-x=\"-8\""
           "     inkscape:window-y=\"-8\""
           "     inkscape:window-maximized=\"1\" />"
           "  <metadata"
           "     id=\"metadata7\">"
           "    <rdf:RDF>"
           "      <cc:Work"
           "         rdf:about=\"\">"
           "        <dc:format>image/svg+xml</dc:format>"
           "        <dc:type"
           "           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />"
           "        <dc:title></dc:title>"
           "      </cc:Work>"
           "    </rdf:RDF>"
           "  </metadata>"
           "  <g"
           "     inkscape:label=\"Layer 1\""
           "     inkscape:groupmode=\"layer\""
           "     id=\"layer1\""
           "     transform=\"translate(1252.3164,-304.67969)\">"
           "    <path"
           "       style=\"opacity:1;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\""
           "       d=\"m -902.3164,304.67969 -28.04007,302.41272 -115.18233,-95.63509 95.63512,115.1823 -302.41272,28.04007 302.41348,28.03931 -95.63588,115.1823 115.18233,-95.63433 28.04007,302.41273 28.03931,-302.41349 115.1823,95.63509 -95.63509,-115.1823 302.41348,-28.03931 -302.41272,-28.04007 95.63433,-115.1823 -115.1823,95.63586 -28.03931,-302.41349 z\""
           "       id=\"path4140\""
           "       inkscape:connector-curvature=\"0\" />"
           "  </g>"
           "</svg>";
}
// ----------------------------------------------------------------------------

ProcessInterface* EPSFBuilderProcess::DefaultInterface() const
{
    return TheEPSFBuilderInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* EPSFBuilderProcess::Create() const
{
    return new EPSFBuilderInstance(this);
}

// ----------------------------------------------------------------------------

ProcessImplementation* EPSFBuilderProcess::Clone(const ProcessImplementation& p) const
{
    const EPSFBuilderInstance* instPtr = dynamic_cast<const EPSFBuilderInstance*>(&p);
    return (instPtr != 0) ? new EPSFBuilderInstance(*instPtr) : 0;
}

// ----------------------------------------------------------------------------

bool EPSFBuilderProcess::NeedsValidation() const
{
    return false;
}

// ----------------------------------------------------------------------------

bool EPSFBuilderProcess::CanProcessCommandLines() const
{
    return false;
}

}	// namespace pcl
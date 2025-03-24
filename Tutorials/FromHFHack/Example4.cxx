#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/ASoAHelpers.h"
#include "Common/DataModel/Multiplicity.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct example4{

	// Define Histogram
	
	HistogramRegistry registry
	{
		"histos",
			{
				{"eta", "eta", {kTH1F, {{160, -4, 4}}}},
				{"mult", "mult", {kTH1F, {{10000, 0, 1000}}}},
				{"h2PtMult", "h2PtMult", {kTH2F, {{10000, 0, 1000}, {800, 0, 8}}}},
			}
	};
	
	// Define filter
	Filter etaFilter = nabs(aod::track::eta) < 1.f;

	void process(soa::Join<aod::Collisions, aod::Mults>::iterator const& Collision, soa::Filtered<aod::Tracks> const& FilteredTracks)
	{
		// Fill in multFV0M
		registry.fill(HIST("mult"), Collision.multFV0M());

		for( auto const& FilteredTrack : FilteredTracks)
		{
			registry.fill(HIST("eta"), FilteredTrack.eta());
			registry.fill(HIST("h2PtMult"), Collision.multFV0M(), FilteredTrack.pt());
		}
	}

};

WorkflowSpec defineDataProcessing( ConfigContext const& cfgc )
{
	return WorkflowSpec{ adaptAnalysisTask<example4>(cfgc)};
}

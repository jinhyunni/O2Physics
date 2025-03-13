#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;

struct example2{

	// Making configurables for histAxisSpec
	Configurable<int> nBins{"eta", 120, "eta"};
	Configurable<float> etaMin{"etaMin", -6., "etaMin"};
	Configurable<float> etaMax{"etaMax", 6., "etaMax"};
	
	// Define histogram registry
	HistogramRegistry registry{ 
		"histos", 
			{
				{"eta", "eta", {HistType::kTH1F, {{nBins, etaMin, etaMax, "eta"}}}}
			}
	};

	void process( aod::Tracks const& tracks )
	{
		for( auto const& track : tracks )
		{
			registry.fill(HIST("eta"), track.eta());
		}

	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec{ adaptAnalysisTask<example2>(cfgc) };
}



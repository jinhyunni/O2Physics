#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;

struct example2of1{

	// Define configurables for histogram
	Configurable<int> nBinsEta{"eta", 120, "eta"};
	Configurable<float> etaMin{"etaMin", -6.0, "etaMin"};
	Configurable<float> etaMax{"etaMax", 6.0, "etaMax"};
	
	Configurable<int> nBinsAvgPt{"nBinsAvgPt", 1000, "nBinsAvgPt"};
	Configurable<float> avgPtMin{"avgPtMin", 0.0, "avgPtMin"};
	Configurable<float> avgPtMax{"avgPtMax", 10.0, "avgPtMax"};

	// Define AxisSpec
	const AxisSpec xAxisEta{nBinsEta, etaMin, etaMax};
	const AxisSpec xAxisAvgPt{nBinsAvgPt, avgPtMin, avgPtMax};
	
	// Define HistogramConfigSpec 
	const HistogramConfigSpec histConfigEta{kTH1F,{xAxisEta}};
	const HistogramConfigSpec histConfigAvgPt{kTH1F, {xAxisAvgPt}};

	// Define HistogramSpec
	HistogramSpec specEta{"eta", "eta", histConfigEta};
	HistogramSpec specAvgPt{"avgPt", "avgPt", histConfigAvgPt};

	// Define HistogramRegistry
	HistogramRegistry registry{"histos", {specEta, specAvgPt}};

	// Process function
	// ! aod::Collision = aod::Collision::iterator
	void process( aod::Collision const& collision, aod::Tracks const& tracks )
	{
		float avgPt = 0;

		for( auto const& track : tracks )
		{
			avgPt += track.pt();
			registry.get<TH1>(HIST("eta")) -> Fill( track.eta() );
		}

		avgPt /= tracks.size();
		LOGP(info, "Event{}, AvgPT: {}", collision.index(), avgPt);

		registry.get<TH1>(HIST("avgPt")) -> Fill( avgPt );
	}

};

WorkflowSpec defineDataProcessing( ConfigContext const& cfgc )
{
	return WorkflowSpec{ adaptAnalysisTask<example2of1>(cfgc)};	
}

// Practice 2
// Use filtering to filter out ...
// 1. particles outside |eta| > 0.8
// 2. particles with DCA > 0.2


#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Common/DataModel/TrackSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct practice2{

	HistogramRegistry registry{
		"histos",
		{
			{"h1eta", "h1eta", {HistType::kTH1F, {{120, -6.0, 6.0}}}},
			{"h1dca", "h1dca", {HistType::kTH1F, {{120, -6.0, 6.0}}}}
		}
	};

	using MyTrack = soa::Join<aod::Tracks, aod::TracksDCA, aod::TracksExtra>;
	
	Filter etaFilter = nabs(aod::track::eta) < 0.8f;
	Filter dcaFilter = nabs(aod::track::dcaXY) < 0.2f;

	using FilteredMyTrack = soa::Filtered<MyTrack>;

	void process(aod::Collision const& collision, FilteredMyTrack& tracks)
	{
		for( auto const& track : tracks )
		{
			//if( track.tpcNClsCrossedRows() < 70 ) continue;
			registry.fill(HIST("h1eta"), track.eta());
			registry.fill(HIST("h1dca"), track.dcaXY());
		}
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<practice2>(cfgc)
	};
}

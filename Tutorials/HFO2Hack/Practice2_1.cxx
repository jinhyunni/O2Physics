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

struct practice2sub1{

	HistogramRegistry registry{
		"histos",
		{
			{"h1eta", "h1eta", {HistType::kTH1F, {{120, -6.0, 6.0}}}},
			{"h1dca", "h1dca", {HistType::kTH1F, {{120, -6.0, 6.0}}}},
			{"h1test_tpcNClsFindable", "h1test_tpcNClsFindable", {HistType::kTH1F, {{200, 0.0, 200.0}}}},
			{"h1test_tpcNClsMinusCrossedRows", "h1test_tpcNClsMinusCrossedRows", {HistType::kTH1F, {{400, -200.0, 200.0}}}}
		}
	};

	using MyTracks = soa::Join<aod::Tracks, aod::TracksDCA, aod::TracksExtra>;

	Filter etaFilter = nabs(aod::track::eta) < 0.8f;
	Filter dcaFilter = nabs(aod::track::dcaXY) < 0.2f;
	//Filter tpcFilter = (aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows) > 70.0f; /*Recipite for calculating tpcNClsCrossedRows() dynamic colums entry*/
	//Filter tpcFilter = ((float)aod::track::tpcNClsFindable - (float)aod::track::tpcNClsFindableMinusCrossedRows) > 70.0f; /*Recipite for calculating tpcNClsCrossedRows() dynamic colums entry*/

	using FilteredMyTracks = soa::Filtered<MyTracks>;

	void process(aod::Collision const& collision, FilteredMyTracks& tracks)
	{
		for( auto const& track : tracks )
		{
			//if( track.tpcNClsCrossedRows() < 70 ) continue;
			registry.fill(HIST("h1test_tpcNClsFindable"), track.tpcNClsFindable());
			registry.fill(HIST("h1test_tpcNClsFindableMinusCrossedRows"), track.tpcNClsFindableMinusCrossedRows());
			registry.fill(HIST("h1eta"), track.eta());
			registry.fill(HIST("h1dca"), track.dcaXY());
		}
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<practice2sub1>(cfgc)
	};
}

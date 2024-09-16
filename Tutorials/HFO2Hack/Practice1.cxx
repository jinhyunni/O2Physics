// Practice 1
// Apply some cuts on reconstructed tracks
// 
// [Eventwise cut]
// * Event with reconstructed MC
//
// [Trackwise cut]
// TrackSelection!
// -> 1. DCA cuts
// -> 2. TPC cuts

#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Common/DataModel/TrackSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct practice1{

	// Define Histogram registry
	HistogramRegistry registry{
		"histos",
		{
			{"h1pT", "h1pT", {HistType::kTH1F, {{100, 0.0, 10.0}}}},
			{"h1eta", "h1eta", {HistType::kTH1F, {{120, -6.0, 6.0}}}},
			{"h1pTWTrackSelection", "h1pTWTrackSelection", {HistType::kTH1F, {{100, 0.0, 10.0}}}},
			{"h1etaWTrackSelection", "h1etaWTrackSelection", {HistType::kTH1F, {{120, -6.0, 6.0}}}},

		}
	};

	// Define processing with generated data
	// =====================================
	using MyCollision = soa::Join<aod::Collisions, aod::McCollisionLabels>::iterator;

	void processGen(MyCollision const& collision, aod::Tracks const& tracks)
	{
		for( auto const& track : tracks)
		{
			registry.fill(HIST("h1pT"), track.pt());
			registry.fill(HIST("h1eta"), track.eta());
		}
	}

	// Define processing with generated data, apply track selection
	// ============================================================
	using TracksSelected = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;
	Configurable<int> tpcCut{"tpcCut", 70, "tpcCut"};
	Configurable<float> dcaCut{"dcaCut", 0.2, "dcaCut"};

	void processGenWTrackSelection(MyCollision const& collision, TracksSelected const& tracks)
	{
		for( auto const& track : tracks)
		{
			if( fabs(track.dcaXY()) > dcaCut ) continue;
			if( track.tpcNClsCrossedRows() < tpcCut ) continue;

			registry.fill(HIST("h1pTWTrackSelection"), track.pt());
			registry.fill(HIST("h1etaWTrackSelection"), track.eta());
		}
	}

	// Adding process switch
	PROCESS_SWITCH(practice1, processGen, "processGenerator", true);
	PROCESS_SWITCH(practice1, processGenWTrackSelection, "processGenWTrackSelection", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<practice1>(cfgc),	
	};
}

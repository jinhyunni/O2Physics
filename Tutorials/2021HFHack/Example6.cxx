// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

// Expressions
#include "Framework/ASoAHelpers.h" // Filters, partitions...

// Tables
#include "Common/DataModel/TrackSelectionTables.h"
#include "Common/DataModel/Multiplicity.h"

#include <iostream>

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;


struct example6a{

	// Slice cache
	SliceCache cache;

	// Define Histogram
	Configurable<int>	nBinEta{"nBinEta", 120, "nBinEta"};	
	Configurable<float> etaMin{"etaMin", -6.0, "etaMin"};	
	Configurable<float> etaMax{"etaMax", +6.0, "etaMax"};

	Configurable<int> nBinDca{"nBinDca", 100, "nBinDca"};
	Configurable<float> dcaMin{"dcaMin", -5.0, "dcaMin"};
	Configurable<float> dcaMax{"dcaMax", +5.0, "dcaMax"};

	Configurable<int> nBinTpc{"nBinTpc", 1000, "nBinTpc"};
	Configurable<float> tpcMin{"tpcMin", 0.0, "tpcMin"};
	Configurable<float> tpcMax{"tpcMax", 500.0, "tpcMax"};

	HistogramRegistry histos{"histos",{}};

	// Initialize
	void init(InitContext const&)
	{
		histos.add("etaMB", "etaMB", {kTH1F, {{nBinEta, etaMin, etaMax}}});		
		histos.add("etaFwd", "etaFwd", {kTH1F, {{nBinEta, etaMin, etaMax}}});		
		histos.add("etaMid", "etaMid", {kTH1F, {{nBinEta, etaMin, etaMax}}});		
		histos.add("dcaFwd", "dcaFwd", {kTH1F, {{nBinDca, dcaMin, dcaMax}}});		
		histos.add("dcaMid", "dcaMid", {kTH1F, {{nBinDca, dcaMin, dcaMax}}});		
		histos.add("tpcFwd", "tpcFwd", {kTH1F, {{nBinTpc, tpcMin, tpcMax}}});		
		histos.add("tpcMid", "tpcMid", {kTH1F, {{nBinTpc, tpcMin, tpcMax}}});	
	}

	// Define tables to consume
	using JoinedCollisions = soa::Join<aod::Collisions, aod::Mults>;
	using JoinedCollision  = JoinedCollisions::iterator;
	
	using JoinedTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;
	using FilteredJoinedTracks = soa::Filtered<JoinedTracks>;

	Filter DcaFilter = nabs(aod::track::dcaXY) < 0.2f;
	//Filter TpcFilter = aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows >= 70;
	
	//Partition<FilteredJoinedTracks> TracksAtFwd = ((aod::track::eta > 2.8f) and (aod::track::eta < 5.1f)) or ((aod::track::eta > -3.7f) and (aod::track::eta < -1.7f));
	
	Configurable<float> etaCutLow{"etaCutLow", -0.8f, "etaCutLow"};	
	Configurable<float> etaCutMax{"etaCutMax", 0.8f, "etaCutMax"};	
	
	Partition<FilteredJoinedTracks> TracksAtFwd = (aod::track::eta > etaCutMax) or (aod::track::eta < etaCutLow);
	Partition<FilteredJoinedTracks> TracksAtMid = (aod::track::eta <= etaCutMax) and (aod::track::eta >= etaCutLow);

	#if 0
	LOGP(info, "Type of JoinedCollisions: {}", std::typeid(JoinedCollisions.name()));
	LOGP(info, "Type of JoinedCollision: {}", std::typeid(JoinedCollision.name()));
	LOGP(info, "Type of JoinedTracks: {}", std::typeid(JoinedTracks.name()));
	LOGP(info, "Type of FilteredJoinedTracks: {}", std::typeid(FilteredJoinedTracks.name()));
	LOGP(info, "Type of TracksFwd: {}", std::typeid(TracksFwd.name()));
	LOGP(info, "Type of TracksMid: {}", std::typeid(TracksMid.name()));
	#endif	

	// Loop
	void process( JoinedCollision const& collision, FilteredJoinedTracks const& tracks )
	{
		auto MidTracks = TracksAtMid->sliceByCached( aod::track::collisionId, collision.globalIndex(), cache );
		auto FwdTracks = TracksAtFwd->sliceByCached( aod::track::collisionId, collision.globalIndex(), cache );

		// Looping over MB tracks
		for( auto const& track : tracks)
		{
			if( track.tpcNClsCrossedRows() < 70 ) continue;

			histos.get<TH1>(HIST("etaMB")) -> Fill(track.eta());
		}

		// Looping over Mid tracks
		for( auto const& track : MidTracks )
		{
			// TPC quality cut!
			if( track.tpcNClsCrossedRows() < 70 ) continue;

			histos.get<TH1>(HIST("etaMid")) -> Fill(track.eta());
			histos.get<TH1>(HIST("dcaMid")) -> Fill(track.dcaXY());
			histos.get<TH1>(HIST("tpcMid")) -> Fill(track.tpcNClsCrossedRows());
		}

		// Looping over Fwd tracks
		for( auto const& track : FwdTracks )
		{
			// TPC quality cut!
			if( track.tpcNClsCrossedRows() < 70 ) continue;

			histos.get<TH1>(HIST("etaFwd")) -> Fill(track.eta());
			histos.get<TH1>(HIST("dcaFwd")) -> Fill(track.dcaXY());
			histos.get<TH1>(HIST("tpcFwd")) -> Fill(track.tpcNClsCrossedRows());
		}
	}
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<example6a>(cfgc),
	};
}

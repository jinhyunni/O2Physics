// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

// Expressions
#include "Framework/ASoAHelpers.h"

// Data Model
#include "Common/DataModel/TrackSelectionTables.h"
#include "Common/DataModel/Multiplicity.h"

// STL
#include <iostream>

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct example6p1{

	// Configurable
	Configurable<int> nBinEta{"nBinEta", 120, "nBinEta"};
	Configurable<float> etaMin{"etaMin", -6.0, "etaMin"};
	Configurable<float> etaMax{"etaMax", +6.0, "etaMax"};

	Configurable<int> nBinDca{"nBinDca", 100, "nBinDca"};
	Configurable<float> dcaMin{"dcaMin", -1.0, "dcaMin"};
	Configurable<float> dcaMax{"dcaMax", +1.0, "dcaMax"};

	Configurable<int> nBinTpc{"nBinTpc", 1000, "nBinTpc"};
	Configurable<float> tpcMin{"tpcMin", 0.0, "tpcMin"};
	Configurable<float> tpcMax{"tpcMax", 500.0, "tpcMax"};

	Configurable<float> etaCutLow{"etaCutLow", -0.8, "etaCutLow"};
	Configurable<float> etaCutMax{"etaCutMax", +0.8, "etaCutMax"};

	// Histogram registry
	HistogramRegistry histos{"histos", {}};

	// Table definition
	// !! Define partition of tables inside of process function
	using JoinedCollisions		= soa::Join<aod::Collisions, aod::Mults>;

	using JoinedTracks			= soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;
	using FilteredJoinedTracks 	= soa::Filtered<JoinedTracks>;
	Filter DcaFilter			= nabs(aod::track::dcaXY) < 0.2f; 

	// Initialize
	void init(InitContext const& cfgc)
	{
		histos.add("eventCounter", "eventCounter", kTH1F, {{1, 0, 1}});

		histos.add("etaMB", "etaMB", kTH1F, {{nBinEta, etaMin, etaMax}});
		histos.add("etaMid", "etaMid", kTH1F, {{nBinEta, etaMin, etaMax}});
		histos.add("etaFwd", "etaFwd", kTH1F, {{nBinEta, etaMin, etaMax}});
		#ifdef SCALE
		histos.add("etaMBScaled", "etaMBScaled", kTH1F, {{nBinEta, etaMin, etaMax}});
		histos.add("etaMidScaled", "etaMidScaled", kTH1F, {{nBinEta, etaMin, etaMax}});
		histos.add("etaFwdScaled", "etaFwdScaled", kTH1F, {{nBinEta, etaMin, etaMax}});
		#endif

		histos.add("dcaMB", "dcaMB", kTH1F, {{nBinDca, dcaMin, dcaMax}});
		histos.add("dcaMid", "dcaMid", kTH1F, {{nBinDca, dcaMin, dcaMax}});
		histos.add("dcaFwd", "dcaFwd", kTH1F, {{nBinDca, dcaMin, dcaMax}});

		histos.add("tpcMB", "tpcMB", kTH1F, {{nBinTpc, tpcMin, tpcMax}});
		histos.add("tpcMid", "tpcMid", kTH1F, {{nBinTpc, tpcMin, tpcMax}});
		histos.add("tpcFwd", "tpcFwd", kTH1F, {{nBinTpc, tpcMin, tpcMax}});
	}

	// Process
	int numDataFrame=0;
	void process(JoinedCollisions const& collisions, FilteredJoinedTracks const& tracks)
	{
		numDataFrame++;
		LOGP(info, "Stored collision number: {}", collisions.size() );
		// Collision loop
		for( auto const& collision : collisions)
		{
			//Fill eventCounter
			histos.get<TH1>(HIST("eventCounter")) -> Fill(0.5);

			// Define Partition
			Partition<FilteredJoinedTracks> MBTracks = 
				(aod::track::collisionId == collision.globalIndex());

			Partition<FilteredJoinedTracks> FwdTracks = 
				(aod::track::collisionId == collision.globalIndex()) and 
				((aod::track::eta < etaCutLow) or (aod::track::eta > etaCutMax));

			Partition<FilteredJoinedTracks> MidTracks = 
				(aod::track::collisionId == collision.globalIndex()) and 
				((aod::track::eta >= etaCutLow) and (aod::track::eta <=  etaCutMax));

			// Binding tables
			MBTracks.bindTable(tracks);
			FwdTracks.bindTable(tracks);
			MidTracks.bindTable(tracks);

			// Loop over MB tracks : No eta selection
			for(auto const& MBTrack : MBTracks)
			{
				if( MBTrack.tpcNClsCrossedRows() < 70 ) continue;

				histos.get<TH1>(HIST("etaMB"))			-> Fill( MBTrack.eta() );
				#ifdef SCALE
				histos.get<TH1>(HIST("etaMBScaled"))	-> Fill( MBTrack.eta() );
				#endif
				histos.get<TH1>(HIST("dcaMB"))			-> Fill( MBTrack.dcaXY() );
				histos.get<TH1>(HIST("tpcMB"))			-> Fill( MBTrack.tpcNClsCrossedRows() );
			}

			// Loop over Fwd tracks
			for(auto const& FwdTrack : FwdTracks)
			{
				if( FwdTrack.tpcNClsCrossedRows() < 70 ) continue;

				histos.get<TH1>(HIST("etaFwd"))			-> Fill( FwdTrack.eta() );
				#ifdef SCALE
				histos.get<TH1>(HIST("etaFwdScaled"))	-> Fill( FwdTrack.eta() );
				#endif
				histos.get<TH1>(HIST("dcaFwd"))			-> Fill( FwdTrack.dcaXY() );
				histos.get<TH1>(HIST("tpcFwd"))			-> Fill( FwdTrack.tpcNClsCrossedRows() );
			}
	
			// Loop over Mid tracks
			for(auto const& MidTrack : MidTracks)
			{
				if( MidTrack.tpcNClsCrossedRows() < 70 ) continue;

				histos.get<TH1>(HIST("etaMid"))			-> Fill( MidTrack.eta() );
				#ifdef SCALE
				histos.get<TH1>(HIST("etaMidScaled"))	-> Fill( MidTrack.eta() );
				#endif
				histos.get<TH1>(HIST("dcaMid"))			-> Fill( MidTrack.dcaXY() );
				histos.get<TH1>(HIST("tpcMid"))			-> Fill( MidTrack.tpcNClsCrossedRows() );
			}

		} // Collision loop
		  
		LOGP(info, "Counted events : {}, used DataFrame: {}", histos.get<TH1>(HIST("eventCounter")) -> GetEntries(), numDataFrame);
	}

	// Scale histogram with event number
	// ! Using histos outside a function(ex. process, init function) will cause an error!
	#if 0 // -> Swich 0 to 1 -> Fail!
	histos.get<TH1>(HIST("etaMBScaled")) -> Scale(1./ histos.get<TH1>(HIST("eventCounter"))->GetEntries());
	histos.get<TH1>(HIST("etaFwdScaled")) -> Scale(1./ histos.get<TH1>(HIST("eventCounter"))->GetEntries());
	histos.get<TH1>(HIST("etaMidScaled")) -> Scale(1./ histos.get<TH1>(HIST("eventCounter"))->GetEntries());
	#endif
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<example6p1>(cfgc),	
	};
}

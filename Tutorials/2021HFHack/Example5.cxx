#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/ASoAHelpers.h"					// -> To use filters
#include "Common/DataModel/TrackSelectionTables.h"	// -> To use aod::TracksDCA
#include "Common/DataModel/Multiplicity.h"			// -> To use "aod::Mults" table
	
using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions; // -> To use expressions such as Join, Filter...

struct example5{

	//Configurables
	Configurable<int> nBinEventCounter{"nBinEC", 1, "nBinEC"};
	Configurable<int> nBinEvtMult{"nBinEvtMult", 10000, "nBinEvtMult"};
	Configurable<int> nBinDca{"nBinDca", 100, "nBinDca"};
	Configurable<int> nBinPt{"nBinPt", 1000, "nBinPt"};
	Configurable<int> nBinEta{"nBinEta", 100, "nBinEta"};

	// Histogram registry
	HistogramRegistry histos=
	{
		"histos",
		{
			{"eventCounter", "eventCounter", {kTH1F, {{nBinEventCounter, 0, 1}}}},
			{"eventMult", "eventMult", {kTH1F, {{nBinEvtMult, 0, 1000}}}},
			{"tpcNClsCrossedRows", "tpcNClsCrossedRows", {kTH1F, {{1000, 0, 1000}}}},
			{"dca", "dca", {kTH1F, {{nBinDca, -1.5, 1.5}}}},
			{"pt", "pt", {kTH1F, {{nBinPt, -5.0, 5.0}}}},
			{"eta", "eta", {kTH1F, {{nBinEta, -5.0, 5.0}}}}
		}
	};

	using JoinedCollisions		= soa::Join<aod::Collisions, aod::Mults>;
	using JoinedCollision 		= JoinedCollisions::iterator;

	using JoinedTracks 			= soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA>;
	using FilteredJoinedTracks	= soa::Filtered<JoinedTracks>;

	// Apply eta filter
	Filter EtaFilter	= nabs(aod::track::eta) < 1.f;
	// Apply DCA filter
	Filter DcaFilter 	= nabs(aod::track::dcaXY) < 0.2f;
	// Apply TPC number of crossed rows -> Not viable with Filter expression
	Filter TpcFilter 	= aod::track::tpcNClsFindable - aod::track::tpcNClsFindableMinusCrossedRows >= 70;
	
	void process( JoinedCollision const& collision, FilteredJoinedTracks const& tracks )
	{
		// Fill in event counter
		histos.get<TH1>(HIST("eventCounter"))	-> Fill(0.5);
		// Fill in event multiplicity
		histos.get<TH1>(HIST("eventMult"))		-> Fill( collision.multFV0M() );

		for( auto const& track : tracks )
		{
			histos.get<TH1>(HIST("tpcNClsCrossedRows"))		-> Fill(track.tpcNClsCrossedRows());
			histos.get<TH1>(HIST("dca"))					-> Fill(track.dcaXY());
			histos.get<TH1>(HIST("pt"))						-> Fill(track.pt());
			histos.get<TH1>(HIST("eta"))					-> Fill(track.eta());
		}
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<example5>(cfgc),	
	};
}


// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

// DataModels
#include "Common/DataModel/TrackSelectionTables.h"

// Expressions
#include "Framework/ASoAHelpers.h"

// namspaces
using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct handson3{

	/*
	 *	Handson3 : Use MC file to calculate...
	 *	
	 * 		Add dcaXY, eta, TPC cut
	 *
	 *		a) pT Resolution
	 *		b) Efficiency(of what?)
	 *		c) Collision reconstruction efficiency
	 */

	// SliceCache
	SliceCache cache;

	// Configurable
	Configurable<int> nBinPt{"nBinPt", 100, "nBinPt"};
	Configurable<float> ptMin{"ptMin", 0.0, "ptMin"};
	Configurable<float> ptMax{"ptMax", 10.0, "ptMax"};

	Configurable<int> nBinDeltaPt{"nBinDeltaPt", 100, "nBinDeltaPt"};
	Configurable<float> deltaPtMin{"deltaPtMin", -1.0, "deltaPtMin"};
	Configurable<float> deltaPtMax{"deltaPtMax", +1.0, "deltaPtMax"};

	// HistogramRegistry
	HistogramRegistry histos{"histos", {}};

	// Making table aliases
	Filter dcaFilter = nabs(aod::track::dcaXY) < 0.2f;

	//using CompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::McTrackLabels, aod::McParticles>;
	// -> This will cause compile error : Becayse aod::Tracks and aod::McParticles have the same column 'pt'
	using CompleteTracks = soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::McTrackLabels>;
	using FilteredCompleteTracks = soa::Filtered<CompleteTracks>;


	// Init
	void init(InitContext const&)
	{
		AxisSpec axisPt{nBinPt, ptMin, ptMax, "p_{T}"};
		AxisSpec axisDeltaPt{nBinDeltaPt, deltaPtMin, deltaPtMax, "#Delta p_{T}"};

		histos.add("PtResolution", "PtResolution", kTH2F, {{axisPt, axisDeltaPt}});

		// Pt histogram of identified tracks 
		histos.add("PtPionTrack", "PtPionTrack", kTH1F, {{axisPt}});
		histos.add("PtKaonTrack", "PtKaonTrack", kTH1F, {{axisPt}});
		histos.add("PtProtonTrack", "PtProtonTrack", kTH1F, {{axisPt}});
	
		// Pt histogram of identified generated tracks
		histos.add("PtPionGen", "PtPionGen", kTH1F, {{axisPt}});
		histos.add("PtKaonGen", "PtKaonGen", kTH1F, {{axisPt}});
		histos.add("PtProtonGen", "PtProtonGen", kTH1F, {{axisPt}});

	}

	// ProcessReco
	// -> FilteredCompletedTracks, aodMcParticle table will be grouped by aod::Collisions::iterator
	void processReco( aod::Collision const& collision, FilteredCompleteTracks const& tracks, aod::McParticles const&)
	{
		for( auto const& track : tracks)
		{
			// Track quality cut
			if( track.tpcNClsCrossedRows() < 70 ) continue;

			if( ! track.has_mcParticle() ) continue;
	
			auto mcParticle = track.mcParticle();
			histos.fill(HIST("PtResolution"), track.pt(), track.pt() - mcParticle.pt());

			if( mcParticle.isPhysicalPrimary() and fabs(mcParticle.y())<0.5 )
			{
				int id = abs(mcParticle.pdgCode());

				if( id == 211 ) histos.fill(HIST("PtPionTrack"), mcParticle.pt());
				else if( id == 321 ) histos.fill(HIST("PtKaonTrack"), mcParticle.pt());
				else if( id == 2212 ) histos.fill(HIST("PtProtonTrack"), mcParticle.pt());
			}
		
		}
	}

	// ProcessGen
	void processGen( aod::McParticles const& particles )
	{
		for( auto const& particle : particles )
		{
			if( particle.isPhysicalPrimary() and fabs(particle.y())<0.5 )
			{
				int id = abs(particle.pdgCode());

				if( id == 211 )	histos.fill(HIST("PtPionGen"), particle.pt());
				else if( id == 321)	histos.fill(HIST("PtKaonGen"), particle.pt());
				else if( id == 2212) histos.fill(HIST("PtProtonGen"), particle.pt());
				else continue;
			}
		}
	}

	// Process switch
	PROCESS_SWITCH(handson3, processReco, "processReco", true);
	PROCESS_SWITCH(handson3, processGen, "processGen", true);

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<handson3>(cfgc),
	};
}

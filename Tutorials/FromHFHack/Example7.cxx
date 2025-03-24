// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

// For Additional data model
#include "Framework/AnalysisDataModel.h" // -> aod::MCTracksLabels
#include "Common/DataModel/TrackSelectionTables.h"

// Expressions
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct example7{

	// Configurable
	Configurable<int> nBinEta{"nBinEta", 1200, "nBinEta"};
	Configurable<float> nBinMin{"nBinMin", -6.0, "nBinMin"};
	Configurable<float> nBinMax{"nBinMax", +6.0, "nBinMax"};

	HistogramRegistry histos{"histos", {}};

	// Alias for Table
	using CompleteTracks
		= soa::Join<aod::Tracks, aod::TracksExtra, aod::TracksDCA, aod::McTrackLabels>;

	using FilteredCompleteTracks
		= soa::Filtered<CompleteTracks>;
	Filter DcaFilter = nabs(aod::track::dcaXY) < 0.2f;

	// Initialization
	void init(InitContext const& cfgc)
	{
		histos.add("mcEta_DCAOff", "mcEta_DCAOff", kTH1F, {{nBinEta, nBinMin, nBinMax}});
		histos.add("mcEta_DCAOn", "mcEta_DCAOn", kTH1F, {{nBinEta, nBinMin, nBinMax}});
	}

	// Process
	void process
	(
	 // Table supscription
	 aod::McCollision const& collision, 
	 CompleteTracks const& tracksOff,
	 FilteredCompleteTracks const& tracksOn,
	 aod::McParticles const&
	)
	{

		// Loop over tracks without DCA cut
		for( auto const& track : tracksOff)
		{
			if( ! track.has_mcParticle() ) continue;

			auto mcParticle = track.mcParticle();
			histos.get<TH1>(HIST("mcEta_DCAOff")) -> Fill(mcParticle.eta());
		}
		
		// Loop over tracks with DCA cut
		for( auto const& track : tracksOn)
			{
				if( ! track.has_mcParticle() ) continue;

				auto mcParticle = track.mcParticle();
				histos.get<TH1>(HIST("mcEta_DCAOn")) -> Fill(mcParticle.eta());
			}
		}


};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<example7>(cfgc)
	};
}

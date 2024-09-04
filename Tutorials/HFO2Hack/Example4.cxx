// Example4. Calculate avgpT per event
// 1. Add process switch
//  
//    -- Add process switch to do the same task with MC particles
//
// 2. Use filter to only track particles within |eta|<0.8





#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"

using namespace o2;
using namespace o2::framework;

struct Example4{

	//Define Histogram registry
	HistogramRegistry registry{"histo", {}};

	// Add filters
	Configurable<float> etaCut{"etaCut", 0.8, "eta cut to track"};
	expressions::Filter etaFilter = aod::track::eta >= -etaCut and aod::track::eta <= etaCut;
	expressions::Filter etaMCFilter = aod::mcparticle::eta >= -etaCut and aod::mcparticle::eta <= etaCut;

	void init(InitContext const&)
	{
		registry.add("hpt", "hpt", {HistType::kTH1F, {{101, -0.05, 10.05}}});
		registry.add("hptMC", "hptMC", {HistType::kTH1F, {{101, -0.05, 10.05}}});
		registry.add("havgpt", "havgpt", {HistType::kTH1F, {{21, -0.05, 2.05}}});
		registry.add("havgptMC", "havgptMC", {HistType::kTH1F, {{101, -0.05, 2.05}}});
	}

	// Subscribe to filtered table!
	void process(aod::Collision const& collision, soa::Filtered<aod::Tracks> const& tracks)
	{
		float avgpt = 0.;

		if( tracks.size() != 0)
		{

			for( auto const& track : tracks )
			{
				avgpt += track.pt();
				registry.fill( HIST("hpt"), track.pt());
			}

			// Divide total summation of pT with total track # in filtered table
			avgpt /= tracks.size();	
			registry.fill(HIST("havgpt"), avgpt);

		}// if event has a track within |eta| <0.8

	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<Example4>(cfgc)
	};
}

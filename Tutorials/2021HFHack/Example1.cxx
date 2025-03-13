#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

struct example1{

	o2::framework::Configurable<int> printrate={"printrate", 2e5, "printrate"};

	/*
	 *	Subscription to 'Tracks' table
	 */
	void process(o2::aod::Tracks const& tracks)
	{
		// Loop over track table
		for(auto const& track : tracks)
		{
			if( track.index() % printrate == 0)
			{
				LOGP(info, "Track {} has pT: {}, eta: {}, phi: {}", track.index(), track.pt(), track.eta(), track.phi());
			}
		}
	}

};

o2::framework::WorkflowSpec defineDataProcessing(o2::framework::ConfigContext const& cfgc)
{
	return o2::framework::WorkflowSpec{ adaptAnalysisTask<example1>(cfgc) };
}

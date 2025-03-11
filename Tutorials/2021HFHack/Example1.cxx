#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

struct example1{

	o2::framework::Configurable<int> rate={"rate", 1e6, "rate"};

	/*
	 *	Subscription to 'Tracks' table
	 */
	void process(o2::aod::Tracks const& tracks)
	{
		// Loop over track table
		for(auto const& track : tracks)
		{
			if( track.index() % rate == 0)
			{
				LOGP(info, "Track {} has pT {}", track.index(), track.pt());
			}
		}
	}

};

o2::framework::WorkflowSpec defineDataProcessing(o2::framework::ConfigContext const& cfgc)
{
	return o2::framework::WorkflowSpec{ adaptAnalysisTask<example1>(cfgc) };
}

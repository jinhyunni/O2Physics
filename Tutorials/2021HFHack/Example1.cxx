#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

struct example1{

	const int rate = 1e3;

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

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec{ adaptAnalysisTask<example1>(cfgc) };
}

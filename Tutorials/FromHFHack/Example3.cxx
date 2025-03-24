#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct example3{
	
	/*
	 *	Define histogram
	 */

	Configurable<int>	nBinEta{"nBinEta", 120, "nBinEta"};
	Configurable<float>	binMinEta{"binMinEta", -6.0, "binMinEta"};
	Configurable<float>	binMaxEta{"binMaxEta", 6.0, "binMaxEta"};

	Configurable<int>	nBinPt{"nBinPt", 1000, "nBinPt"};
	Configurable<float>	binMinPt{"binMinPt", 0.0, "binMinPt"};
	Configurable<float>	binMaxPt{"binMaxPt", 10.0, "binMaxPt"};




	HistogramRegistry registry=
	{
		"histos",
		{
			{"eta", "eta", {kTH1F, {{nBinEta, binMinEta, binMaxEta}}}},
			{"pt", "pt", {kTH1F, {{nBinPt, binMinPt, binMaxPt}}}},
			{"avgPt", "avgPt", {kTH1F, {{nBinPt, binMinPt, binMaxPt}}}},
		}
	};

	/*
	 *	Define filter:
	 *		Only select particles within |eta|<1 range
	 *
	 *	Alias filtered table
	 *		Use 'using' syntax
	 */
	Filter etaFilter = nabs(aod::track::eta) < 1.f;
	using FilteredTracks = soa::Filtered<aod::Tracks>;

	/*
	 *	Group filtered  tracks with same collision id
	 */
	
	void process(aod::Collision const&, FilteredTracks const& filteredTracks)
	{
		float avgPt = 0;

		if( filteredTracks.size() != 0 )
		{

		//for( auto const& track : filteredTracks )
		for( auto trackIt = filteredTracks.begin(); trackIt != filteredTracks.end(); trackIt++ )
		{
			auto const& track = *trackIt;

			registry.get<TH1>(HIST("eta")) -> Fill( track.eta() );
			registry.get<TH1>(HIST("pt")) -> Fill( track.pt() );
			avgPt += track.pt();
		}

		avgPt /= filteredTracks.size();

		registry.get<TH1>(HIST("avgPt")) -> Fill(avgPt);
		}
	};

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec{adaptAnalysisTask<example3>(cfgc)};
}

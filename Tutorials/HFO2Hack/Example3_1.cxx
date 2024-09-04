#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;

// Subscribe to TrackIU data only
struct trackperdataframe{

	// Define Hisgoram registry
	HistogramRegistry registry{"histos", {}};

	// Define configurable for pT axis
	Configurable<int> pTnBins{"pTnBins", 100, "pTnBins"};
	Configurable<float> pTmin{"pTmin", 0., "pTmin"};
	Configurable<float> pTmax{"pTmax", 10., "pTmax"};

	// Define configurable for event counter
	Configurable<int> eventnBins{"eventBins", 1, "eventBins"};
	Configurable<float> eventmin{"eventmin", 0., "eventmin"};
	Configurable<float> eventmax{"eventmax", 1., "eventmax"};

	void init(InitContext const&)
	{
		AxisSpec pTAxis{pTnBins, pTmin, pTmax};
		AxisSpec eventAxis{eventnBins, eventmin, eventmax};
		registry.add("pTIU", "pTIU", {HistType::kTH1F, {pTAxis}});
		registry.add("eventCounter", "eventCounter", {HistType::kTH1F, {eventAxis}});
	}

	void process(aod::TracksIU const& tracksIU)
	{
		if( tracksIU.size() != 0 )
		{
			registry.fill( HIST("eventCounter"), 0.5);
		}

		for( auto const& trackIU : tracksIU)
		{
			registry.fill(HIST("pTIU"), trackIU.pt());
		}
	}

};

// Subscribe to TrackIU & Collision table
struct trackperevent{
	
	// Define Histogram registry
	HistogramRegistry registry{"histos", {}};

	// Define configurable for pT axis
	Configurable<int> pTnBins{"pTnBins", 100, "pTnBins"};
	Configurable<float> pTmin{"pTmin", 0., "pTmin"};
	Configurable<float> pTmax{"pTmax", 10., "pTmax"};

	// Define configurable for event counter
	Configurable<int> eventnBins{"eventBins", 1, "eventBins"};
	Configurable<float> eventmin{"eventmin", 0., "eventmin"};
	Configurable<float> eventmax{"eventmax", 1., "eventmax"};

	void init(InitContext const&)
	{
		AxisSpec pTAxis{pTnBins, pTmin, pTmax};
		AxisSpec eventAxis{eventnBins, eventmin, eventmax};
		registry.add("pTIU", "pTIU", {HistType::kTH1F, {pTAxis}});
		registry.add("eventCounter", "eventCounter", {HistType::kTH1F, {eventAxis}});
	}

	void process(aod::Collision const& collision, aod::TracksIU const& tracksIU)
	{
		registry.fill( HIST("eventCounter"), 0.5);

		for( auto const& trackIU : tracksIU)
		{
			registry.fill( HIST("pTIU"), trackIU.pt() );
		}
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<trackperdataframe>(cfgc),
		adaptAnalysisTask<trackperevent>(cfgc)
	};
}

// Practice 3
// Practice PID!

// Default include files
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"

// headerfiles for PID
#include "Common/DataModel/PIDResponse.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Make 2D histogram
// x axis: track momentum
// y axis: N sigma of particle

struct practice3{

	HistogramRegistry registry{"histos",
		{
			{"h2PNsigEl", "h2PNsigEl", {HistType::kTH2F,{{100, 0.0, 10.0}, {200, -10.0, 10.0}}}}, // Electron
			{"h2PNsigMu", "h2PNsigMu", {HistType::kTH2F,{{100, 0.0, 10.0}, {200, -10.0, 10.0}}}}, // Muon
			{"h2PNsigPr", "h2PNsigPr", {HistType::kTH2F,{{100, 0.0, 10.0}, {200, -10.0, 10.0}}}}, // Proton
			{"h2PNsigKa", "h2PNsigKa", {HistType::kTH2F,{{100, 0.0, 10.0}, {200, -10.0, 10.0}}}}, // Kaon
			{"h2PNsigPi", "h2PNsigPi", {HistType::kTH2F,{{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},  // pion

			
			{"h1TofMass", "h1TofMass", {HistType::kTH1F,{{300, 0.0, 3.0}}}}  // pion
		}
	};

	using TPCTracks = soa::Join<aod::Tracks,
		  aod::pidTPCEl/*PID with TPC*/,
		  aod::pidTPCMu,
		  aod::pidTPCPr, 
		  aod::pidTPCKa, 
		  aod::pidTPCPi>;
	
	void processTPC(TPCTracks const& tracks)
	{
		for( auto const& track : tracks)
		{
			registry.fill(HIST("h2PNsigEl"), track.p(), track.tpcNSigmaEl());
			registry.fill(HIST("h2PNsigMu"), track.p(), track.tpcNSigmaMu());
			registry.fill(HIST("h2PNsigPr"), track.p(), track.tpcNSigmaPr());
			registry.fill(HIST("h2PNsigKa"), track.p(), track.tpcNSigmaKa());
			registry.fill(HIST("h2PNsigPi"), track.p(), track.tpcNSigmaPi());
		}
	}

	using TOFTracks = soa::Join< aod::Tracks,
		  aod::pidTOFmass,
		  aod::pidTOFbeta>;

	void processTOF( TOFTracks const& tracks)
	{
		for(auto const& track : tracks)
		{
			registry.fill(HIST("h1TofMass"), track.mass());
		}
	}
	
	//ProcessSwitch
	PROCESS_SWITCH(practice3, processTPC, "processTOF", true);
	PROCESS_SWITCH(practice3, processTOF, "processTPC", true);
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<practice3>(cfgc)
	};
}

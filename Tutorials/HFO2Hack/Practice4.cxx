// Practice4 : Use TOF & TPC Full table
//          -> Use Full table to access full N-sigma value
//
// To get this, subscribe to these tables produced by helper-tasks
//
// [TOF]
// o2::aod::pidTOFFullEl
// o2::aod::pidTOFFullMu
// ...
//
// [TPC]
// o2::aod::pidTPCFullEl
// o2::aod::pidTPCFullMu
// ...

// Default headerfiles
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"

// Headerfile for PID
#include "Common/DataModel/PIDResponse.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct checkPID{

	// Histogram registry
	// ==================
	HistogramRegistry registry{
		"histos",
		{
			// TPC PID
			{"h2PNSigEl_TPC", "h2PNSigEl_TPC", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigMu_TPC", "h2PNSigMu_TPC", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigPr_TPC", "h2PNSigPr_TPC", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigKa_TPC", "h2PNSigKa_TPC", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigPi_TPC", "h2PNSigPi_TPC", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			
			// TOF PIC
			{"h2PNSigEl_TOF", "h2PNSigEl_TOF", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigMu_TOF", "h2PNSigMu_TOF", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigPr_TOF", "h2PNSigPr_TOF", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigKa_TOF", "h2PNSigKa_TOF", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}},
			{"h2PNSigPi_TOF", "h2PNSigPi_TOF", {HistType::kTH2F, {{100, 0.0, 10.0}, {200, -10.0, 10.0}}}}
		}
	};

	// Fill TPC PID Information
	// ========================

	using MyTPCPIDTracks = soa::Join<
						aod::Tracks,
						aod::pidTPCFullEl,
						aod::pidTPCFullMu,
						aod::pidTPCFullPr,
						aod::pidTPCFullKa,
						aod::pidTPCFullPi
						>;

	void processTPC(MyTPCPIDTracks const& tracks)
	{
		for( auto const& track : tracks)
		{
			registry.fill(HIST("h2PNSigEl_TPC"), track.p(), track.tpcNSigmaEl());
			registry.fill(HIST("h2PNSigMu_TPC"), track.p(), track.tpcNSigmaMu());
			registry.fill(HIST("h2PNSigPr_TPC"), track.p(), track.tpcNSigmaPr());
			registry.fill(HIST("h2PNSigKa_TPC"), track.p(), track.tpcNSigmaKa());
			registry.fill(HIST("h2PNSigPi_TPC"), track.p(), track.tpcNSigmaPi());
		}
	}

	// Make process switch
	PROCESS_SWITCH(checkPID, processTPC, "processTPC", true);
	
	// Fill TPC PID Information
	// ========================

	using MyTOFPIDTracks = soa::Join<
						aod::Tracks,
						aod::pidTOFFullEl,
						aod::pidTOFFullMu,
						aod::pidTOFFullPr,
						aod::pidTOFFullKa,
						aod::pidTOFFullPi
						>;

	void processTOF(MyTOFPIDTracks const& tracks)
	{
		for( auto const& track : tracks )
		{
			registry.fill(HIST("h2PNSigEl_TOF"), track.p(), track.tofNSigmaEl());
			registry.fill(HIST("h2PNSigMu_TOF"), track.p(), track.tofNSigmaMu());
			registry.fill(HIST("h2PNSigPr_TOF"), track.p(), track.tofNSigmaPr());
			registry.fill(HIST("h2PNSigKa_TOF"), track.p(), track.tofNSigmaKa());
			registry.fill(HIST("h2PNSigPi_TOF"), track.p(), track.tofNSigmaPi());
		}
	}

	// Make process switch
	PROCESS_SWITCH(checkPID, processTOF, "processTOF", true);
	
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<checkPID>(cfgc),
	};
}

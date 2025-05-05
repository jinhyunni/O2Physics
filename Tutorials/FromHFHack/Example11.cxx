// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

//DataModels
#include "Common/DataModel/TrackSelectionTables.h"

// Expressions
#include "Framework/ASoA.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

/*
 *	Accessing another table's information via index table
 *
 *	1. Make index table
 *	2. Access to objects which has index tables
 *
 *	For practice, let's make a index table which stores global index of tracks
 *	which satisfies quality cuts
 *		a. DCA cut
 *		b. # of TPC Crossed rows
 *		c. |eta| < 1.0
 */

namespace o2::aod
{

namespace goodtrack
{
DECLARE_SOA_INDEX_COLUMN_FULL(GoodTrack, goodTrack, int, aod::Tracks, "_goodtrk"); // -> Column name will be 'GoodTrack'Id
}

DECLARE_SOA_TABLE(GoodTracks, "AOD", "GOODTRACKS",
		o2::soa::Index<>,
		goodtrack::GoodTrackId);
}

struct fillIndexTable {

	Produces<aod::GoodTracks> cursor;

	struct : ConfigurableGroup {
	
		Configurable<float> etaCut{"EtaCut", 1.0, "EtaCut"};
		Configurable<float> dcaCut{"dcaCut", 0.5, "dcaCut"};
		Configurable<int> tpcCut{"tpcCut", 70, "tpcCut"};

	} configs;

	// Alises for table uses
	using MyTracks = soa::Join<aod::Tracks, aod::TracksDCA, aod::TracksExtra>;
	using MyTrack = MyTracks::iterator;

	void init(InitContext const&){}

	void process(MyTracks const& tracks, aod::Collisions const&)
	{
		for (const auto& track : tracks) {

			if (!track.has_collision()) 
				continue;
			if (fabs(track.eta()) > configs.etaCut)
				continue;
			if (track.dcaXY() > configs.dcaCut)
				continue;
			if (track.tpcNClsCrossedRows() < configs.tpcCut)
				continue;

			cursor(track.globalIndex());

			auto collision = track.collision_as<aod::Collisions>();
			int collisionId = collision.globalIndex();
			if (collisionId % 10000 == 0) {
				if (track.globalIndex() % 10000 == 0) {
					LOGP(info, "Collision {} processed : Track{}", collisionId, track.globalIndex());
				}
			}
		}
	}

};
#if 1 
struct consumeIndexTable {

	HistogramRegistry registry{"histos", {}};

	struct : ConfigurableGroup {

		Configurable<int> nBinPt{"nBinPt", 100, "nBinPt"};
		Configurable<float> ptLe{"ptLE", 0.0, "ptLE"};
		Configurable<float> ptHe{"ptHE", 10.0, "ptHE"};

		Configurable<int> nBinEta{"nBinEta", 120, "nBinEta"};
		Configurable<float> etaLe{"etaLE", -3.0, "etaLE"};
		Configurable<float> etaHe{"etaHE", +3.0, "etaHE"};

		Configurable<int> nBinDca{"nBinDca", 120, "nBinDca"};
		Configurable<float> dcaLe{"dcaLE", -3.0, "dcaLE"};
		Configurable<float> dcaHe{"dcaHE", +3.0, "dcaHE"};

		Configurable<int> nBinTpcNCls{"nBinTpcNCls", 1000, "nBinTpcNCls"};
		Configurable<float> tpcNClsLe{"tpcNClsLE", 0.0, "tpcNClsLE"};
		Configurable<float> tpcNClsHe{"tpcNClsHE", 200.0, "tpcNClsHE"};

	} configs;

	void init(InitContext const&)
	{
		registry.add("ptSkimmedTrack", "ptSkimmedTrack", kTH1F, {{configs.nBinPt, configs.ptLe, configs.ptHe}});
		registry.add("etaSkimmedTrack", "etaSkimmedTrack", kTH1F, {{configs.nBinEta, configs.etaLe, configs.etaHe}});
		registry.add("dcaSkimmedTrack", "dcaSkimmedTrack", kTH1F, {{configs.nBinDca, configs.dcaLe, configs.dcaHe}});
		registry.add("tpcNClsSkimmedTrack", "tpcNClsSkimmedTrack", kTH1F, {{configs.nBinTpcNCls, configs.tpcNClsLe, configs.tpcNClsHe}});
	}

	using AodFullTracks = soa::Join<aod::Tracks, aod::TracksDCA, aod::TracksExtra>;

	void process(aod::GoodTracks const& skimmedTracks, AodFullTracks const&)
	{
		for (auto const& skimmedTrack : skimmedTracks) {
		
			auto aodTrack = skimmedTrack.goodTrack_as<AodFullTracks>();

			registry.get<TH1>(HIST("ptSkimmedTrack")) -> Fill(aodTrack.pt());
			registry.get<TH1>(HIST("etaSkimmedTrack")) -> Fill(aodTrack.eta());
			registry.get<TH1>(HIST("dcaSkimmedTrack")) -> Fill(aodTrack.dcaXY());
			registry.get<TH1>(HIST("tpcNClsSkimmedTrack")) -> Fill(aodTrack.tpcNClsCrossedRows());
		}
	};

};
#endif
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<fillIndexTable>(cfgc),
		adaptAnalysisTask<consumeIndexTable>(cfgc),
	};
}

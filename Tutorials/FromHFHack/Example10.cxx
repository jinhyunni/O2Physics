// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"

// DataModels
#include "Common/DataModel/Multiplicity.h"

// Expressions
#include "Framework/ASoA.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

/*
 *	This example is dedicated for practical use of index tables(or columns)
 *
 *	-> Fill average pT per collision
 *	-> Use Preslice to group particles which has the same collision id.
 *	-> Do this by multiplicity?
 *
 *	-> Multiplicity vs pt
 */

using MyCollisions = soa::Join<aod::Collisions, aod::Mults>;
using MyCollision = MyCollisions::iterator;

struct example10 {
	
	struct : ConfigurableGroup {

		Configurable<bool> switchAvgPtPerColl{"switchAvgPtPerCollision", true, "switchAvgPtPerCollision"};
		Configurable<bool> switchPtVsMult{"switchPtVsMult", true, "swithcPtVsMult"};

		Configurable<int> nBinPt{"nBinPt", 100, "nBinPt"};
		Configurable<float> ptLow{"pTLow", 0.0, "pTLow"};
		Configurable<float> ptHigh{"pTHigh", 10.0, "pTHigh"};

		Configurable<int> nBinMult{"nBinMult", 1000, "nBinMult"};
		Configurable<float> multLow{"multLow", 0.0, "multLow"};
		Configurable<float> multHigh{"multHigh", 100, "multHigh"};

	} configs;

	HistogramRegistry hist{"histos", {}};

	void init(InitContext const&)
	{
		hist.add("avgPtPerCollision", "avgPtPerCollision", {kTH1F, {{configs.nBinPt, configs.ptLow, configs.ptHigh}}});
		hist.add("avgPtPerCollisionByMult", "avgPtPerCollisionByMult", {kTH2F, {{configs.nBinMult, configs.multLow, configs.multHigh}, {configs.nBinPt, configs.ptLow, configs.ptHigh}}});
		hist.add("ptVsMult", "ptVsMult", {kTH2F, {{configs.nBinMult, configs.multLow, configs.multHigh}, {configs.nBinPt, configs.ptLow, configs.ptHigh}}});
	}

	Preslice<aod::Tracks> tracksPerCollision = aod::track::collisionId;

	void processAvgPtPerCollision(aod::Tracks const& tracks, MyCollisions const& collisions)
	{
		// group tracks by collision
		for( auto const& collision : collisions )
		{
			auto thisCollId = collision.globalIndex();
			auto groupedTracks = tracks.sliceBy(tracksPerCollision, thisCollId);

			auto numOfTracks = groupedTracks.size();
			float avgPt = 0;

			for(auto const& track : groupedTracks)
			{
				avgPt += track.pt();
			}

			avgPt /= numOfTracks;

			hist.get<TH1>(HIST("avgPtPerCollision")) -> Fill(avgPt);
			hist.get<TH2>(HIST("avgPtPerCollisionByMult")) -> Fill(collision.multFV0M(), avgPt);

		}
	}
	PROCESS_SWITCH(example10, processAvgPtPerCollision, "Calculate average pt per collision", configs.switchAvgPtPerColl);

	void processPtVsMult(aod::Tracks const& tracks, MyCollisions const& colls)
	{
		for (const auto& track : tracks) {
			if( !track.has_collision() ) continue;
			auto collision = track.collision_as<MyCollisions>();
			hist.get<TH2>(HIST("ptVsMult")) -> Fill(collision.multFV0M(), track.pt());
		}
	}
	PROCESS_SWITCH(example10, processPtVsMult, "Calculate pt vs multiplicity", configs.switchPtVsMult);

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<example10>(cfgc),
	};
};

// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

// Data models
#include "Common/DataModel/Multiplicity.h"

// Expressions
#include "Framework/ASoA.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

using CollisionWLabels = soa::Join<aod::Collisions, aod::McCollisionLabels>; //->Pre-definded tables 

// Make McMults tables
namespace o2::aod
{
namespace multsgen
{
DECLARE_SOA_COLUMN(McMult, mcMult, int);
} // multsgen
DECLARE_SOA_TABLE(McMults, "AOD", "Mult counted at generation level", multsgen::McMult);
} // o2::aod

// Make index table which connects McMults with McCollision tables
namespace o2::aod
{
namespace idx
{
DECLARE_SOA_INDEX_COLUMN(CollisionWLabel, collision); // -> 's' is automatically added at the end
DECLARE_SOA_INDEX_COLUMN(McCollision, mccollision); // -> This is needed because there it cannot be joined with Collisions table
} // idx
DECLARE_SOA_INDEX_TABLE_USER(MatchedMCRec, McCollisions, "MMCR", idx::McCollisionId, idx::CollisionWLabelId);
} // o2::aod

// Task to fill tables
struct filltable{

	Produces<aod::McMults> mcmults;
	Builds<aod::MatchedMCRec> idx; // -> This builds index column

	using v0mMcParticles = soa::Filtered<aod::McParticles>;
	Filter v0mFilter
		= (aod::mcparticle::eta > 2.7f and aod::mcparticle::eta < 5.1f)
		or (aod::mcparticle::eta > -3.7f and aod::mcparticle::eta < -1.7f);

	// Task to fill table
	void process(aod::McCollision const& collision, v0mMcParticles const& particles)
	{
		int count = 0;
		for( auto const& particle : particles )
		{
			if( particle.isPhysicalPrimary() ){
				count++;
			}
		}

		mcmults(count);
	}
};

struct example9{

	HistogramRegistry histos{"histos", {}};

	void init(InitContext const& cfgc)
	{
		histos.add("mcmultMatched", "mcmultMatched", kTH1F, {{10000, 0, 10000}});
	}

	using MatchedMcCollisions = soa::Join<aod::McCollisions, aod::McMults, aod::MatchedMCRec>;
	
	void process( MatchedMcCollisions const& collisions )
	{
		for(auto const& collision : collisions)
		{
			// If MatchedMcCollisions doens't have corresponding collision, then pass
			if(!collision.has_collision()){
				return;
			}

			histos.fill(HIST("mcmultMatched"), collision.mcMult());
		}
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<filltable>(cfgc),
		adaptAnalysisTask<example9>(cfgc),
	};
}

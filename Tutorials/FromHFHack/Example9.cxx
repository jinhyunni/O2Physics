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

using CollisionsWMcLabelsFull = soa::Join<aod::Collisions, aod::McCollisionLabels>;

using CollisionsWMcLabels = soa::SmallGroups<soa::Join<aod::Collisions, aod::McCollisionLabels>>; //-> Pre-definded tables
																								  //-> Cannot be subscribed alone 
using CollisionWMcLabels = CollisionsWMcLabels::iterator;

#if 0
// Make index table which connects McMults with McCollision tables
namespace o2::aod
{
namespace idx
{
DECLARE_SOA_INDEX_COLUMN(CollisionWMcLabel, collision); // -> 's' is automatically added at the end
DECLARE_SOA_INDEX_COLUMN(McCollision, mccollision); // -> This is needed because there it cannot be joined with Collisions table
} // idx
DECLARE_SOA_INDEX_TABLE_USER(MatchedMCRec, aod::McCollisions, "MMCR", idx::McCollisionId, idx::CollisionWLabelId);
} // o2::aod
#endif

/*
 *	Concept of this tutorial
 *
 *	Matching McCollisions with Collisions
 */

// Make McMults tables
namespace o2::aod
{
namespace multsgen
{
DECLARE_SOA_COLUMN(MultGen, multgen, int);
} // multsgen
DECLARE_SOA_TABLE(MultsGen, "AOD", "Mult counted at generation level", multsgen::MultGen);
} // o2::aod

// Task to printout
struct printouttable1{

	void init(InitContext& cfgc){}

	void process(aod::McCollisions const& collisions)
	{
		for( auto const& collision : collisions )
		{
			LOGP(info, "Check1 : McCollision global index : {}", collision.globalIndex());
		}

	}

};

// Task to printout
struct printouttable2{

	void init(InitContext& cfgc){}
	
	void process( aod::McCollision const& mcCollision,
				  CollisionsWMcLabels const& collisions ) // -> SmallGroped table grouped by McCollision iterator
	{
		if( collisions.size() < 1 ){
			LOGP(info, "! Generated event doesn't have any reconstructed events !");
			return;
		} else if ( collisions.size() > 2 ) {
			LOGP(info, "! Generated event has multiple reconstructed event : {} !", collisions.size());
		}



		for( auto const& collision : collisions )
		{
			LOGP(info, "Check2 : McCollision's id via indexing {}", collision.mcCollisionId());
		}
	}

};

// Task to fill tables
// Calculated multiplicites by only using the generated tracks which has corresponding reco
struct filltable{
	
	Produces<aod::MultsGen> mcmults;

	using v0mMcParticles = soa::Filtered<aod::McParticles>;

	Filter v0mFilter
		= (aod::mcparticle::eta > 2.7f and aod::mcparticle::eta < 5.1f)
		or (aod::mcparticle::eta > -3.7f and aod::mcparticle::eta < -1.7f);

	HistogramRegistry histos{"histos", {}};

	void init(InitContext const& cfgc)
	{
		histos.add("FilledParticleEta", "FilledParticleEta", kTH1F, {{240, -6, 6}});
	}
	
	// Task to fill table
	void process(aod::McCollision const& GenCollision, CollisionsWMcLabels const& RecoCollisions, v0mMcParticles const& particles)
	{
		if( RecoCollisions.size() < 1 ){
			return;
		}

		for( auto const& collision : RecoCollisions )
		{
			// Print out
			// LOGP(info, "GenCollision Id : {}/ Reco collision Id : {}", GenCollision.globalIndex(), collision.globalIndex());
			int count=0;
			for( auto const& particle : particles )
			{
				if( particle.isPhysicalPrimary() ){
					histos.fill(HIST("FilledParticleEta"), particle.eta());
					count++;
				}
			}

			mcmults(count);
		}
	}
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<printouttable1>(cfgc),
		adaptAnalysisTask<printouttable2>(cfgc),
		adaptAnalysisTask<filltable>(cfgc)
	};
}

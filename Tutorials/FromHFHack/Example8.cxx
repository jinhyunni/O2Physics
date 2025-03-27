// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

// Data Models

// Expressionsa
#include "Framework/ASoA.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Derived table declaration
// ! namespace -> Declare with small letter
namespace o2::aod
{
namespace multgen // -> Name space to delcare Column
{
DECLARE_SOA_COLUMN(MultGen, multGen, int);
} // namespace MultsGen
DECLARE_SOA_TABLE(MultsGen, "AOD", "Multiplicity at Generation level", multgen::MultGen);
} // namspace o2::aod

// Task for filling derived table 
struct pretask{

	Configurable<int> printRate{"printRate", 200, "printRate"};

	// Define cursor to be used for filling derived table
	Produces<aod::MultsGen> cursor;

	// Alias for Table used to fill derived table
	// ! Nothing to be joined or partitioned for McCollision table
	using FilteredMcParticles= soa::Filtered<aod::McParticles>;

	Filter v0mFilter = 
		(aod::mcparticle::eta>2.8f and aod::mcparticle::eta<5.1f) // V0A acceptance
		or
		(aod::mcparticle::eta>-3.7f and aod::mcparticle::eta<-1.7f); // V0C acceptance
	
	// Process
	void process( aod::McCollision const& collision , FilteredMcParticles const& particles )
	{
		// Particles are grouped with McCollision iterator
		// -> Process function performed per collision
		// -> Multiplicity for each collision can be calculated
		
		int count = 0;
		
		for( auto const& particle : particles )
		{
			if(particle.isPhysicalPrimary())
			{
				count++;
			}
		}
		
		cursor(count);

		if( collision.globalIndex() % printRate == 0 )
		{
			LOGP(info, "Collision{} has multiplicity{}", collision.globalIndex(), count);
		}
	}

};

struct example8{
	
	HistogramRegistry histos{"histos", {}};

	void init(InitContext const& cfgc)
	{
		histos.add("mcmult", "mcmult", kTH1F, {{10000, 0, 10000}});
	}

	using JoinedMcCollisions = soa::Join<aod::McCollisions, aod::MultsGen>;

	void process(JoinedMcCollisions const& collisions)
	{
		for( auto const& collision : collisions )
		{
			histos.fill(HIST("mcmult"), collision.multGen());
		}
	}

};

// Task to perform actual analysis


WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<pretask>(cfgc),
		adaptAnalysisTask<example8>(cfgc),
	};
}


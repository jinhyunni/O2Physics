// Example7: Making Index table and using them
// Objective: Only fill MC Collisions that have reconstructed counterpart!

#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Common/DataModel/Multiplicity.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Making tables
// =============
// Make a table which contains Multiplicity info
// calculated by counting # of tracks in certain eta region
//
// Counting region:
//		
//		Forward :: -3.7f ~ -1.7f
//      Backward:: 2.7f ~ 5.1f

namespace o2
{
namespace aod
{
namespace mcmult
{
DECLARE_SOA_COLUMN(MultGen/*ColumnName*/, multgen/*Getter*/, int/*Type*/);
}//namespace mcmult
DECLARE_SOA_TABLE(MultsGen/*TableName*/, "AOD"/*Origin*/, "MultsGen"/*Description*/, mcmult::MultGen/*ColumnName1, ColumnName2, ...*/);
}// namespcae aod
}// namespace o2

// Task, Fill table
// ===============
struct FillTable{

	// Declare Produces<T> fuction
	Produces<aod::MultsGen/*Table*/> tableGen;

	//Define filter for collecting tracks in certain eta region
	//using mctrack = aod::mcparticle;
	Filter etaFilterMC = (aod::mcparticle::eta/*eta(Column) of each mc pariticle defined under mctrack namespace*/ > -3.7f and aod::mcparticle::eta < -1.7f) or (aod::mcparticle::eta > 2.7f and aod::mcparticle::eta < 5.1f);
	
	//Calculating multiplicity of mc tracks in certain eta region by collision in MC data
	void process( aod::McCollision const& collision, soa::Filtered<aod::McParticles> const& particles)
	{
		int multInCertainEta = 0;

		for( auto const& particle : particles )
		{
			if( particle.isPhysicalPrimary() )
			{
				multInCertainEta++;
			}
		}// McParticles table loop

		//Fill each row of generated table
		tableGen(multInCertainEta);
	}
	
};


// Task, Main analysis
// ===================
struct Example7{
	
};

// Define Workflow Spec
// ===================
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<FillTable>(cfgc),
		adaptAnalysisTask<Example7>(cfgc)
	};
}

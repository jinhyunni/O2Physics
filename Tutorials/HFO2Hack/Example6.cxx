// Example6 : Making tables and consuming them
//
// 1. Make a derived data tables for MC collisions...why?
//    -> task added by processSwitch cannot be haded to the next pipelined helper task...
//    -> so...make a new table...?

#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Common/DataModel/Multiplicity.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Define new Table

//1. Define Column
//   -> Add multiplicity information to MC Collision table
//   -> Manually count particles in certain eta region...
//   -> DECLARE_SOA_COLUMN(Name, Getter, Type);
namespace o2::aod::mcmult
{
DECLARE_SOA_COLUMN(MULTFV0gen, multfv0m, int);
}

// 2. Define Table
//    -> DECLARE_SOA_TABLE(Name, Origin, Description, Column1, Column2, Column3...)

namespace o2::aod
{
DECLARE_SOA_TABLE(
		MultsGen, 
		"AOD", 
		"MultGen", 
		mcmult::MULTFV0gen);
}

// Fill generated table with a task
struct PreTask{

	// 3. Fill each row of table
	//    -> Produces<origin::of::produced::TableGen> name;
	Produces<aod::MultsGen> mult;
	
	// Add filter in certain eta region
	Filter etaCutMC = 	( aod::mcparticle::eta > 2.7 and aod::mcparticle::eta < 5.1 ) or
						( aod::mcparticle::eta > -3.7 and aod::mcparticle::eta < -1.7 );

	// Subscribe to MCcollision iterator, and McParticles table to iterate
	// Fill each colums of rows by the following way:
	// -> name(Column1, Column2, Column3,...)
	void process(aod::McCollision const& collision, soa::Filtered<aod::McParticles> const& particles)
	{	
		int count = 0;
		for( auto const& particle : particles )
		{
			if(particle.isPhysicalPrimary()) 
			{
				count++;
			}
		}

		// Fill each row's column!
		mult(count);
	}
};

// Main task will subscribe to the generated tables
// MC-Collision table will be joint with MultsGen table, created in this analysis task!
struct Example6{
		
	//Histogram registry
	HistogramRegistry registry{
		"histos",
		{
			// Histograms based on data
			{"h1pT", "Data based pT histogram", {HistType:: kTH1F, {{101, -0.05, 10.05}}}},
			{"h1avgpT", "Data based <pT> histogram", {HistType:: kTH1F, {{21, -0.05, 2.05}}}},
			{"h1eta", "Data based eta histogram", {HistType:: kTH1F, {{60, -3, 3}}}},
			{"h2pTMult", "Data based pT vs multiplicity  histogram", {HistType:: kTH2F, {{101, -0.05, 10.05}, {201, -0.5, 200.5}}}},
			{"h2avgpTMult", "Data based <pT> vs multiplicity  histogram", {HistType:: kTH2F, {{21, -0.05, 2.05}, {201, -0.5, 200.5}}}},

			// Histograms based on generated level
			{"h1pTMC", "MC-Data based pT histogram", {HistType:: kTH1F, {{101, -0.05, 10.05}}}},
			{"h1avgpTMC", "MC-Data based <pT> histogram", {HistType:: kTH1F, {{21, -0.05, 2.05}}}},
			{"h1etaMC", "MC-Data based eta histogram", {HistType:: kTH1F, {{60, -3, 3}}}},
			{"h2pTMultMC", "MC-Data based pT vs multiplicity  histogram", {HistType:: kTH2F, {{101, -0.05, 10.05}, {201, -0.5, 200.5}}}},
			{"h2avgpTMultMC", "MC-Data based <pT> vs multiplicity  histogram", {HistType:: kTH2F, {{21, -0.05, 2.05}, {201, -0.5, 200.5}}}}

		}
	};

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<PreTask>(cfgc),
		adaptAnalysisTask<Example6>(cfgc),
	};
}

// Example5-1: Use Partitioning
// Objective : Make 2D histogram of pT(avgpT) vs FV0M multiplicity
//           +) Use Partitioning -> For making generated MC multiplicity table
//           +) Joint table -> For getting information about FT0M multiplicity in reconstructed data
//           +) Make a table which stores multiplicity in FT0A & FT0M eta range for generated data
//
// Twists: Use FT0M Multiplicity -> Defined in Mults table for reconstructed data, but not for generated data
//         FT0A: 3.8 <= eta <= 5.0
//         FT0C: -3.4 <= eta <= -2.3

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Common/DataModel/Multiplicity.h"
#include "Common/DataModel/TrackSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Make a table for counting Multiplicity in FT0A & FT0M eta for generated table


namespace o2::aod
{

namespace multgen
{
DECLARE_SOA_COLUMN(MultGen/*Name*/, multgen/*getter*/, int/*Type*/); 
} // namespace multgen

DECLARE_SOA_TABLE(MultsGen/*Name*/, "AOD"/*Origin*/, "MultsGen"/*Description*/, multgen::MultGen/*Column1Name*/);

} // namespace o2::aod

// Decalre iterator to MC multiplicity table
using multgen = o2::aod::MultsGen::iterator;

// Fill the defined table
// ======================

// Use partitioning to make a particle table in FT0M acceptance
// +) Check if mc particles is physical primary in the partitioning declaration stage
// Make histogram to check counted particle's eta

struct filltable{

	// Define a cursor to fill prodeced table
	Produces<aod::MultsGen> cursor;
	SliceCache cache;

	// Define Table partition
	Partition<aod::McParticles> ParticlesFT0M = ( ( (aod::mcparticle::eta>=3.8f) and (aod::mcparticle::eta<=5.0f) )   // FT0A
                                             or ( (aod::mcparticle::eta>=-3.4f) and (aod::mcparticle::eta<=-2.3f) ) )// FT0C
	                                        and ( aod::mcparticle::flags & (uint8_t)o2::aod::mcparticle::enums::PhysicalPrimary ) == (uint8_t)o2::aod::mcparticle::enums::PhysicalPrimary;  // check if the particle is physical primary
	                                        //and (aod::mcparticle::enums::PhysicalPrimary == true);  // check if the particle is physical primary
	// Define Histograms
	HistogramRegistry registry{
		"histos",
		{
			{"h1NotPhysicalPrimary", "h1NotPhysicalPrimary", {HistType::kTH1F, {{1, 0.0, 1.0}}}},
			{"h1EtaInFT0M", "h1EtaInFT0M", {HistType::kTH1F, {{120, -6.0, 6.0}}}},
			{"h1MultFT0M", "h1MultFT0M", {HistType::kTH1F, {{300, 0.0, 300.0}}}}
		}
	};

	// Fill Histogram
	void process(aod::McCollision const& mccollision/*Doesn't explicitly declare subcription variable?*/, aod::McParticles const&)
	{
		// Using partitioned sub-frame of McParticles table
		int mult = 0;

		// Manually group partitioned McParticles table with McCollision!
		auto assoTracksThisCollision = ParticlesFT0M -> sliceByCached(aod::mcparticle::mcCollisionId, mccollision.globalIndex(), cache);

		if( mccollision.globalIndex() % 100 == 0 )
		{
			LOGP(info, "McCollision: {}, Number of Tracks filled: {}",mccollision.globalIndex(), assoTracksThisCollision.size());
		}

		for(auto const& particle : assoTracksThisCollision)
		{
			if( particle.isPhysicalPrimary() )
			{
				registry.fill(HIST("h1EtaInFT0M"), particle.eta());
				mult++;
			}

			else if( ! particle.isPhysicalPrimary() )
			{
				registry.fill(HIST("h1NotPhysicalPrimary"), 0.5);
			}
		}

		// Fill h1MultFT0M
		registry.fill(HIST("h1MultFT0M"), mult);
		// Fill Table row -> Make it joinalble with McCollisions
		cursor(mult);
	}
};

// Cross check
// ===========
struct crosscheck{
	
	// Define Histogram registry
	// =========================
	HistogramRegistry registry{"histos", {}};

	Configurable<int> nBinCheck{"nBinCheck", 1, "nBinCheck"};
	Configurable<float> minCheck{"minCheck", 0.0, "minCheck"};
	Configurable<float> maxCheck{"maxCheck", 1.0, "maxCheck"};

	Configurable<int> nBinEta{"nBinEta", 120, "nBinEta"};
	Configurable<float> minEta{"minEta", -6.0, "minEta"};
	Configurable<float> maxEta{"maxEta", 6.0, "maxEta"};

	Configurable<int> nBinMult{"nBinMult", 300, "nBinMult"};
	Configurable<float> minMult{"minMult", 0, "minMult"};
	Configurable<float> maxMult{"maxMult", 300.0, "maxMult"};

	// Init 
	// ====
	void init(InitContext const&)
	{
		// Define histogram specs
		AxisSpec checkAxis{nBinCheck, minCheck, maxCheck};
		AxisSpec etaAxis{nBinEta, minEta, maxEta};
		AxisSpec multAxis{nBinMult, minMult, maxMult};

		// Add histograms
		registry.add("h1Check", "h1Check", kTH1F, {checkAxis});
		registry.add("h1Eta", "h1Eta", kTH1F, {etaAxis});
		registry.add("h1Mult", "h1Mult", kTH1F, {multAxis});
		registry.add("h1ConditionErrCheck", "h1ConditionErrCheck", kTH1F, {checkAxis});
	}

	// Define process
	Filter particleWithinFT0M = (aod::mcparticle::eta >= 3.8f and aod::mcparticle::eta <= 5.0f)
                              or(aod::mcparticle::eta >= -3.4f and aod::mcparticle::eta <= -2.3f);

	void process(aod::McCollision const&, soa::Filtered<aod::McParticles> const& particles)
	{
		int mult = 0;

		for(auto const& particle : particles)
		{
			if( ! particle.isPhysicalPrimary() )
			{
				registry.fill(HIST("h1Check"), 0.5);
			}

			else if( particle.isPhysicalPrimary())
			{
				registry.fill(HIST("h1Eta"), particle.eta());
				mult++;
			}
		}
	
		registry.fill(HIST("h1Mult"), mult);
	}

	// Process to check error
	// ======================
	void processCheckErr(aod::McCollision const& collision, aod::McParticles const& particles)
	{
		for( auto const& particle : particles)
		{
			if( (! particle.isPhysicalPrimary()) and (particle.eta() >= 3.8f and particle.eta() <= 5.0f))
			{
				registry.fill(HIST("h1ConditionErrCheck"), 0.5);
			}
		}
	}

	// Add Process switch
	// ==================
	PROCESS_SWITCH(crosscheck, processCheckErr, "processCheckErr", true);

};	


// Main analysis
// =============
struct example5sub1{
	
	// Hisgtogram registry
	// ===================
	HistogramRegistry registry{
		"histos", 
		{
			// Define histograms to store reconstructed data
			{"h1pT", "h1pT", {HistType::kTH1F, {{101, -0.05, 10.05}}}},
			{"h1eta", "h1eta", {HistType::kTH1F,  {{120, -6.0, 6.0}}}},
			{"h1Mult", "h1Mult", {HistType::kTH1F,  {{300, 0.0, 300.0}}}},
			{"h2pTMult", "h2pTMult", {HistType::kTH2F,  {{300, 0.0, 300.0}, {101, -0.05, 10.05}}}},
			{"h2avgpTMult", "h2avgpTMult", {HistType::kTH2F,  {{300, 0.0, 300.0}, {21, -0.05, 2.05}}}},

			// Define histograms to store generated particles
			{"h1pTMC", "h1pTMC", {HistType::kTH1F, {{101, -0.05, 10.05}}}},
			{"h1etaMC", "h1etaMC", {HistType::kTH1F,  {{120, -6.0, 6.0}}}},
			{"h1MultMC", "h1MultMC", {HistType::kTH1F,  {{300, 0.0, 300.0}}}},
			{"h2pTMultMC", "h2pTMultMC", {HistType::kTH2F,  {{300, 0.0, 300.0}, {101, -0.05, 10.05}}}},
			{"h2avgpTMultMC", "h2avgpTMultMC", {HistType::kTH2F,  {{300, 0.0, 300.0}, {21, -0.05, 2.05}}}},
		}
	};
	
	// Use partitioning to fill |eta| < 0.8;
	Configurable<float> etaCut{"etaCut", 0.8, "etaCut"};
	//Configurable<float> dcaCut{"dcaCut", 0.2, "dcaCut"};
	//Configurable<int>tpcCut{"tpcCut", 70, "tpcCut"};

	// Join Collisions tables and Mults table
	using MyCollision = soa::Join<aod::Collisions, aod::Mults>::iterator;

	// Partitioning Tracks table
	// apply...
	//    1. Eta cut

	Partition<aod::Tracks> MidTracks = nabs(aod::track::eta) < etaCut;

	// Define process function
	// =======================
	SliceCache cache;
	void processGen(MyCollision const& collision, aod::Tracks const&)
	{
		float avgpT=0.0f;

		// Group partitioned tracks table to each collision
		auto assoMidTracks = MidTracks -> sliceByCached(aod::track::collisionId, collision.globalIndex(), cache);

		for(auto const& track : assoMidTracks)
		{
			registry.fill(HIST("h1pT"), track.pt());
			registry.fill(HIST("h1eta"), track.eta());
			registry.fill(HIST("h2pTMult"), collision.multFT0M(), track.pt());
			avgpT += track.pt();
		}
	
		// Fill FT0M multiplicity
		registry.fill(HIST("h1Mult"), collision.multFT0M());
	
		if( assoMidTracks.size())
		{
			avgpT /= assoMidTracks.size();
			registry.fill(HIST("h2avgpTMult"), collision.multFT0M(), avgpT); 
		}
	}

	// Define MC process function
	// ==========================
	SliceCache cacheMC;

	using GenCollision = soa::Join< aod::McCollisions, aod::MultsGen>::iterator;
	Partition<aod::McParticles> MidEtaParticle = nabs(aod::mcparticle::eta) < etaCut;	
	
	void processMC(GenCollision const& collision, aod::McParticles const& particles)
	{
		// Group Partitioned table with collision
		auto assoMcParticles = MidEtaParticle -> sliceByCached(aod::mcparticle::mcCollisionId, collision.globalIndex(), cacheMC);

		float avgpT = 0.; 

		for(auto const& particle : assoMcParticles)
		{
			avgpT += particle.pt();
			registry.fill(HIST("h1pTMC"), particle.pt());
			registry.fill(HIST("h1etaMC"), particle.eta());
			registry.fill(HIST("h2pTMultMC"), collision.multgen(), particle.pt());
		}

		registry.fill(HIST("h1MultMC"), collision.multgen());

		if( assoMcParticles.size())
		{
			avgpT /= assoMcParticles.size();
			registry.fill(HIST("h2avgpTMultMC"), collision.multgen(), avgpT); 
		}
	}

	//Define processSwithc
	PROCESS_SWITCH(example5sub1, processGen, "processGen", true);
	PROCESS_SWITCH(example5sub1, processMC, "processMC", true);
};

// Define workflow
// ===============
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec
  {
	  adaptAnalysisTask<filltable>(cfgc),
	  adaptAnalysisTask<crosscheck>(cfgc),
      adaptAnalysisTask<example5sub1>(cfgc)
  };
}

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


// Joining Collisions table with McCollisionsLabels, to generate key
using CollisionWLabels = soa::Join<aod::Collisions, aod::McCollisionLabels>;

namespace o2
{

namespace aod
{

namespace multrecon
{
DECLARE_SOA_COLUMN(MultRecon, multrecon, int);
}// namespace multrecon

DECLARE_SOA_TABLE(MultsRecon, "AOD", "MultsRecon", multrecon::MultRecon);

namespace multgen
{
DECLARE_SOA_COLUMN(MultGen, multgen, int);
}// namespace multgen

DECLARE_SOA_TABLE(MultsGen, "AOD", "MultsGen", multgen::MultGen);

namespace idx
{
DECLARE_SOA_INDEX_COLUMN(CollisionWLabel, collision);
DECLARE_SOA_INDEX_COLUMN(McCollision, mccollision);
}// namespcae idx

DECLARE_SOA_INDEX_TABLE_USER(MatchedMCRec, aod::McCollisions, "MMCR", idx::McCollisionId, idx::CollisionWLabelId);
/*
 	Key table for this example
 
	Objective of this example: Making table index for making a connection from generated data to reconstruction data.

	CollisionWLables = soa::Join<aod::Collision, aod::McCollisionLabels> -> Reconstructed data with index to mc generated(aod::McCollisions)data
	McCollisions -> Table with mc generated data

	Use McCollisions table as key table!
 */

}// namespace aod

}// namespace o2


// Task, Fill table
// ===============
struct Filltable{

	//Decalre Builds<T> function to build index table
	Builds<aod::MatchedMCRec> idx;

	// Declare Produces<T> fuction
	Produces<aod::MultsRecon/*Table*/> tableRecon;		// Makes multiplicity table with recon data
	Produces<aod::MultsGen/*Table*/> tableGen;	// Makes multiplicity table with generated data

	//Define filter for collecting tracks in certain eta region
	Filter etaFilter = (aod::track::eta/*eta(Column) of each mc pariticle defined under mctrack namespace*/ > -3.7f and aod::track::eta < -1.7f) or (aod::track::eta > 2.7f and aod::track::eta < 5.1f);
	Filter etaFilterMC = (aod::mcparticle::eta/*eta(Column) of each mc pariticle defined under mctrack namespace*/ > -3.7f and aod::mcparticle::eta < -1.7f) or (aod::mcparticle::eta > 2.7f and aod::mcparticle::eta < 5.1f);

	// Define histogram
	// These histograms are for checking particle's eta distibution, which are used to count multiplicity
	HistogramRegistry registry{
		"CheckHistos",
		{

			{"h1Recon", "h1Recon", {HistType::kTH1F, {{1, 0., 1.}}}},
			{"h1Gen", "h1Recon", {HistType::kTH1F, {{1, 0., 1.}}}},
			{"h1etaRecon", "h1etaRecon", {HistType::kTH1F, {{120, -6.0, 6.0}}}},
			{"h1etaGen", "h1etaGen", {HistType::kTH1F, {{120, -6.0, 6.0}}}}
		}
	};


	//Calculating multiplicity of mc tracks in certain eta region by collision in reconstructed  data

	using MyTrackRecon = soa::Filtered<aod::Tracks>;

	void processRecon(aod::Collision const& , MyTrackRecon const& tracks)
	{
		int multInCertainEta = 0;

		for(auto const& track : tracks)
		{
			multInCertainEta++;
			registry.fill(HIST("h1etaRecon"), track.eta());
		}
		
		registry.fill(HIST("h1Recon"), 0.5);
		tableRecon(multInCertainEta);
	}

	//Calculating multiplicity of mc tracks in certain eta region by collision in MC data

	using MyTrackGen = soa::Filtered<aod::McParticles>;

	void processMC(aod::McCollision const& , MyTrackGen const& particles)
	{
		int multInCertainEta = 0;

		for( auto const& particle : particles )
		{
			if( particle.isPhysicalPrimary() )
			{
				multInCertainEta++;
				registry.fill(HIST("h1etaGen"), particle.eta());
			}
		}// McParticles table loop
		
		registry.fill(HIST("h1Gen"), 0.5);
		tableGen(multInCertainEta);
	}

	// Make process switch
	PROCESS_SWITCH(Filltable, processRecon, "processRecon", true);
	PROCESS_SWITCH(Filltable, processMC, "processMC", true);
};

// Task, Main analysis
// ===================
struct Example7{

	// Define histogram registry
	HistogramRegistry registry{
		
		"histos",
		{
			// Histograms with reconstructed data
			{"h1pT", "h1pT", {HistType::kTH1F, {{101, -0.05, 10.05}}}},
			{"h1avgpT", "h1avgpT", {HistType::kTH1F, {{21, -0.05, 2.05}}}},
			{"h1eta", "h1eta", {HistType::kTH1F, {{60, -3.0, 3.0}}}},
			{"h1mult", "h1mult", {HistType::kTH1F, {{201, -0.05, 200.5}}}},
			{"h2pTmult", "h2pTmult", {HistType::kTH2F, {{101, -0.05, 10.05}, {201, -0.05, 200.5}}}},
			{"h2avgpTmult", "h2avgpTmult", {HistType::kTH2F, {{21, -0.05, 2.05}, {201, -0.05, 200.5}}}},

			// Histograms with generated data
			{"h1pTMC", "h1pTMC", {HistType::kTH1F, {{101, -0.05, 10.05}}}},
			{"h1avgpTMC", "h1avgpTMC", {HistType::kTH1F, {{21, -0.05, 2.05}}}},
			{"h1etaMC", "h1etaMC", {HistType::kTH1F, {{60, -3.0, 3.0}}}},
			{"h1multMC", "h1multMC", {HistType::kTH1F, {{201, -0.05, 200.5}}}},
			{"h2pTmultMC", "h2pTmultMC", {HistType::kTH2F, {{101, -0.05, 10.05}, {201, -0.05, 200.5}}}},
			{"h2avgpTmultMC", "h2avgpTmultMC", {HistType::kTH2F, {{21, -0.05, 2.05}, {201, -0.05, 200.5}}}}

		}
	};

	// Store pt, avgpt, eta, mult information in recon data
	// ====================================================
	// Only use tracks in |eta| < 0.8 
	Configurable<float> etaCut{"etaCut", 0.8, "etaCut"};
	Filter etaFilter = nabs(aod::track::eta) < etaCut;

	using RecoCollision = soa::Join<aod::Collisions, aod::MultsRecon>::iterator;
	using MyTracks = soa::Filtered<aod::Tracks>;

	void process(RecoCollision const& collision, MyTracks const& tracks)
	{
		float avgpT = 0.;
		
		registry.fill(HIST("h1mult"), collision.multrecon());

		for( auto const& track : tracks)
		{
			avgpT += track.pt();
			registry.fill(HIST("h1pT"), track.pt());
			registry.fill(HIST("h1eta"), track.eta());
			registry.fill(HIST("h2pTmult"), track.pt(), collision.multrecon());
		}

		if(tracks.size() != 0)
		{
			registry.fill(HIST("h1avgpT"), avgpT);
			registry.fill(HIST("h2avgpTmult"), avgpT, collision.multrecon());
		}
	}

	// Store pt, avgpt, eta, mult information in generated data
	// ========================================================
	// only use tracks in |eta| < 0.8
	// Use derived table -> MC table with multiplicity information
	//                   -> Only collect data which corresponds to recon data.

	// Making a process switch
	// PROCESS_SWITCH(Example7, processMC, "Process GeneratedData", true);
};

// Define Workflow Spec
// ===================
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<Filltable>(cfgc),
		adaptAnalysisTask<Example7>(cfgc)
	};
}

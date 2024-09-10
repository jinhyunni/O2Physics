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
	Produces<aod::MultsGen/*Table*/> tableGen;	// Makes multiplicity table with generated data

	//Define filter for collecting tracks in certain eta region
	Filter etaFilterMC = (aod::mcparticle::eta/*eta(Column) of each mc pariticle defined under mctrack namespace*/ > -3.7f and aod::mcparticle::eta < -1.7f) or (aod::mcparticle::eta > 2.7f and aod::mcparticle::eta < 5.1f);

	// Define histogram
	// These histograms are for checking particle's eta distibution, which are used to count multiplicity
	HistogramRegistry registry{
		"CheckHistos",
		{
			{"h1Gen", "h1Recon", {HistType::kTH1F, {{1, 0., 1.}}}},
			{"h1etaGen", "h1etaGen", {HistType::kTH1F, {{120, -6.0, 6.0}}}}
		}
	};


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
	PROCESS_SWITCH(Filltable, processMC, "processMC", true);
};

// Make Minimum bias distribution
struct Minimumbias{

	HistogramRegistry registry{
	
		"MBHistos",
		{
			{"h1pT", "h1pT", {kTH1F, {{101, -0.05, 10.05}}}},
			{"h1pTFwd", "h1pTFwd", {kTH1F, {{101, -0.05, 10.05}}}},
			{"h1pTMC", "h1pTMC", {kTH1F, {{101, -0.05, 10.05}}}},
			{"h1eta", "h1eta", {kTH1F, {{120, -6.0, 6.0}}}},
			{"h1etaFwd", "h1etaFwd", {kTH1F, {{120, -6.0, 6.0}}}},
			{"h1etaMC", "h1etaMC", {kTH1F, {{120, -6.0, 6.0}}}}
		}
	};

	// Fill minimumbias distrubutions, Reconstructed data
	void processRecon(aod::Collision const&, aod::Tracks const& tracks, aod::FwdTracks const& fwdtracks)
	{
		for(auto const& track : tracks)
		{
			registry.fill(HIST("h1pT"), track.pt());
			registry.fill(HIST("h1eta"), track.eta());
		}

		for(auto const& fwdtrack : fwdtracks)
		{
			registry.fill(HIST("h1pTFwd"), fwdtrack.pt());
			registry.fill(HIST("h1etaFwd"), fwdtrack.eta());
		}
	}

	// Fill minimumbias distribution, Generated data
	void processGen(aod::McCollision const&, aod::McParticles const& particles)
	{
		for(auto const& particle : particles)
		{
			if(particle.isPhysicalPrimary())
			{
				registry.fill(HIST("h1pTMC"), particle.pt());
				registry.fill(HIST("h1etaMC"), particle.eta());
			}
		}
	}

	// Add ProcessSwitch
	PROCESS_SWITCH(Minimumbias, processRecon, "processRecon", true);
	PROCESS_SWITCH(Minimumbias, processGen, "processGen", true);
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

	using RecoCollision = soa::Join<aod::Collisions, aod::Mults>::iterator;
	using MyTracks = soa::Filtered<aod::Tracks>;

	void processRecon(RecoCollision const& collision, MyTracks const& tracks)
	{
		float avgpT = 0.;
		
		registry.fill(HIST("h1mult"), collision.multFT0M());

		for( auto const& track : tracks)
		{
			avgpT += track.pt();
			registry.fill(HIST("h1pT"), track.pt());
			registry.fill(HIST("h1eta"), track.eta());
			registry.fill(HIST("h2pTmult"), track.pt(), collision.multFT0M());
		}

		if(tracks.size() != 0)
		{
			avgpT /= tracks.size();
			registry.fill(HIST("h1avgpT"), avgpT);
			registry.fill(HIST("h2avgpTmult"), avgpT, collision.multFT0M());
		}
	}

	// Store pt, avgpt, eta, mult information in generated data
	// ========================================================
	// only use tracks in |eta| < 0.8
	// Use derived table -> MC table with multiplicity information
	//                   -> Only collect data which corresponds to recon data.

	Filter etaCutGen = nabs(aod::mcparticle::eta) < etaCut;

	using MyMcCollision = soa::Join<aod::McCollisions, aod::MultsGen, aod::MatchedMCRec>::iterator; // Matched MC data with reconstructed data
	using MyParticles = soa::Filtered<aod::McParticles>;

	void processGen(MyMcCollision const& matchedcollision, MyParticles const& particles)
	{
		// check if index table have getter
		if(! matchedcollision.has_collision()){
			return;
		}

		float avgpT = 0.;

		for( auto const& particle : particles)
		{
			if(particle.isPhysicalPrimary())
			{
				avgpT += particle.pt();
				registry.fill(HIST("h1pTMC"), particle.pt());
				registry.fill(HIST("h1etaMC"), particle.eta());
				registry.fill(HIST("h1multMC"), matchedcollision.multgen());
				registry.fill(HIST("h2pTmultMC"), particle.pt(), matchedcollision.multgen());
			}
		}

		if( particles.size() != 0 )
		{
			avgpT /= particles.size();
			registry.fill(HIST("h1avgpTMC"), avgpT);
			registry.fill(HIST("h2avgpTmultMC"), avgpT, matchedcollision.multgen() );
		}
	}

	// Making a process switch
	PROCESS_SWITCH(Example7, processRecon, "Process ReconstructedData", true);
	PROCESS_SWITCH(Example7, processGen, "Process GeneratedData", true);
};

// Define Workflow Spec
// ===================
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<Filltable>(cfgc),
		adaptAnalysisTask<Minimumbias>(cfgc),
		adaptAnalysisTask<Example7>(cfgc)
	};
}

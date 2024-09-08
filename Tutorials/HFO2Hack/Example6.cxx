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
// ================

namespace o2
{
namespace aod
{
// 1. Define Column
// ================
//   -> Add multiplicity information to MC Collision table
//   -> Manually count particles in certain eta region...
//   -> DECLARE_SOA_COLUMN(Name, Getter, Type);
namespace mcmult
{
DECLARE_SOA_COLUMN(MULTgen, multgen, int);
}

// 2. Define Table
// ===============
//    -> DECLARE_SOA_TABLE(Name, Origin, Description, Column1, Column2, Column3...)
DECLARE_SOA_TABLE(MultsGen, "AOD", "MultGen", mcmult::MULTgen);

} // namespace aod
} // namespace o2

// Fill generated table with a task
struct PreTask{

	// 3. Fill each row of table
	//    -> Produces<origin::of::produced::TableGen> name;
	Produces<aod::MultsGen> mult;
	
	// Add filter in certain eta region
	Filter etaCutMC = 	( aod::mcparticle::eta > 2.7f and aod::mcparticle::eta < 5.1f ) or
						( aod::mcparticle::eta > -3.7f and aod::mcparticle::eta < -1.7f );

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
		
	// Histogram registry
	// ==================
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
			{"h1MultMC", "MC-Data based Multiplicity histo", {HistType::kTH1F, {{300, 0, 300}}}},
			{"h1etaMC", "MC-Data based eta histogram", {HistType:: kTH1F, {{60, -3, 3}}}},
			{"h2pTMultMC", "MC-Data based pT vs multiplicity  histogram", {HistType:: kTH2F, {{101, -0.05, 10.05}, {201, -0.5, 200.5}}}},
			{"h2avgpTMultMC", "MC-Data based <pT> vs multiplicity  histogram", {HistType:: kTH2F, {{21, -0.05, 2.05}, {201, -0.5, 200.5}}}},

			// Histograms based on generated level, applied filter on generated multiplicity
			{"h1pTMCHigh", "MC-Data based pT histogram", {HistType:: kTH1F, {{101, -0.05, 10.05}}}},
			{"h1avgpTMCHigh", "MC-Data based <pT> histogram", {HistType:: kTH1F, {{21, -0.05, 2.05}}}},
			{"h1MultMCHigh", "MC-Data based Multiplicity histo", {HistType::kTH1F, {{300, 0, 300}}}},
			{"h1etaMCHigh", "MC-Data based eta histogram", {HistType:: kTH1F, {{60, -3, 3}}}},
			{"h2pTMultMCHigh", "MC-Data based pT vs multiplicity  histogram", {HistType:: kTH2F, {{101, -0.05, 10.05}, {201, -0.5, 200.5}}}},
			{"h2avgpTMultMCHigh", "MC-Data based <pT> vs multiplicity  histogram", {HistType:: kTH2F, {{21, -0.05, 2.05}, {201, -0.5, 200.5}}}}


		}
	};

	// Subscribe to joined, filterd tables
	// ===================================

	// apply filter in data file
	Configurable<float> etaCut{"etaCut", 0.8, "etaCut"};
	Filter etaFilter = nabs(aod::track::eta) < etaCut;

	using MyCollision = soa::Join<aod::Collisions, aod::Mults>::iterator;
	using MyTrack = soa::Filtered<aod::Tracks>;

	void process(MyCollision const& collision, MyTrack const& tracks)
	{
		float avgpT = 0.;

		for( auto const& track : tracks)
		{
			avgpT += track.pt();
			registry.fill(HIST("h1pT"), track.pt());
			registry.fill(HIST("h1eta"), track.eta());
			registry.fill(HIST("h2pTMult"), track.pt(), collision.multFV0M());
		}

		if(tracks.size() != 0)
		{
			avgpT /= tracks.size();
			registry.fill(HIST("h1avgpT"), avgpT);
			registry.fill(HIST("h2avgpTMult"), avgpT, collision.multFV0M());
		}
	}

	// Subscribe to joined, filtered tables in MC
	//===========================================

	// apply filter in MC data file
	Configurable<float> etaCutMC{"etaCutMC", 0.8, "etaCutMC"};
	Filter etaMCFilter = nabs(aod::mcparticle::eta) < etaCutMC;

	using MyMcCollision = soa::Join<aod::McCollisions, aod::MultsGen>::iterator;
	using MyMCParticles = soa::Filtered<aod::McParticles>;

	void processMC(MyMcCollision const& mccollision, MyMCParticles const& particles)
	{
		float avgpTMC = 0.;

		for( auto const& particle : particles)
		{
			if( particle.isPhysicalPrimary())
			{
				avgpTMC += particle.pt();
				registry.fill(HIST("h1pTMC"), particle.pt());
				registry.fill(HIST("h1etaMC"), particle.eta());
				registry.fill(HIST("h2pTMultMC"), particle.pt(), mccollision.multgen());	// Used defined column getter : .multfv0m()
			}
		}

		if( particles.size() != 0 )
		{
			avgpTMC /= particles.size();
			registry.fill(HIST("h1avgpTMC"), avgpTMC);
			registry.fill(HIST("h2avgpTMultMC"), avgpTMC, mccollision.multgen());
		}

		// Fill generated multiplicity
		registry.fill(HIST("h1MultMC"), mccollision.multgen());
	}

	// Subscribe to filtered-Derived table
	// -> only contain MC particle's in high-multiplicity
	// -> High-multiplicity cut: 100 counts in new derived table.
	//===========================================================
	Configurable<int> highMultCut{"highMultCut", 100, "HighMultiplicityCut"};

	// Filtering Deried table
	Filter highMultFilterMC = aod::mcmult/*namespace of generated column*/::multgen > highMultCut;
	
	// Making a joined table
	using JoinedTable = soa::Join<aod::McCollisions, aod::MultsGen>;
	// applying filter to joined table
	using FilteredJoinedTable = soa::Filtered<JoinedTable>::iterator;

	void processMCHighMult(FilteredJoinedTable const& mccollision, MyMCParticles const& particles)
	{

		float avgpTMCHigh = 0.;

		for( auto const& particle : particles)
		{
			if( particle.isPhysicalPrimary())
			{
				avgpTMCHigh += particle.pt();
				registry.fill(HIST("h1pTMCHigh"), particle.pt());
				registry.fill(HIST("h1etaMCHigh"), particle.eta());
				registry.fill(HIST("h2pTMultMCHigh"), particle.pt(), mccollision.multgen());
			}
		}

		if( particles.size() != 0 )
		{
			avgpTMCHigh /= particles.size();
			registry.fill(HIST("h1avgpTMCHigh"), avgpTMCHigh);
			registry.fill(HIST("h2avgpTMultMCHigh"), avgpTMCHigh, mccollision.multgen());
		}

		// Fill generated multiplicity
		registry.fill(HIST("h1MultMCHigh"), mccollision.multgen());
	}

	// Adding process switch
	PROCESS_SWITCH(Example6, processMC, "processMC", true);
	PROCESS_SWITCH(Example6, processMCHighMult, "processMC with highMult", true);

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<PreTask>(cfgc),
		adaptAnalysisTask<Example6>(cfgc),
	};
}

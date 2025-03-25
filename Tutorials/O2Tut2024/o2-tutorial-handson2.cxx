// Mandatory
#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"

// Expressions
#include "Framework/ASoAHelpers.h"

// For consuming derived table
#include "Tutorials/Skimming/DataModel/DerivedExampleTable.h"

using namespace o2;
using namespace o2::aod;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Define partition outside process loop
// Use SliceCache
struct handson2ver0{

	// Slice cache to perform table partitioning
	SliceCache cache;

	// Declaing configurable
	Configurable<int>	nBinCollisionZ{"nBinCollisionZ", 400, "nBinCollisionZ"};
	Configurable<float> collisionZMin{"collisionZMin", -20, "collisionZMin"};
	Configurable<float> collisionZMax{"collisionZMax", +20, "collisionZMax"};
	

	Configurable<int>	nBinPt{"nBinPt", 400, "nBinPt"};
	Configurable<float>	ptMin{"ptMin", 0, "ptMin"};
	Configurable<float>	ptMax{"ptMax", 0, "ptMax"};

	Configurable<int> nBinDelPhi{"nBinDelPhi", 100, "nBinDelPhi"};
	Configurable<float> delPhiMin{"delPhiMin", -0.5*TMath::Pi(), "delPhiMin"};
	Configurable<float> delPhiMax{"delPhiMax", +1.5*TMath::Pi(), "delPhiMax"};

	Configurable<int> nBinDelEta{"nBinDelEta", 100, "nBinDelEta"};
	Configurable<float> delEtaMin{"delEtaMin", -1.0, "delEtaMin"};
	Configurable<float> delEtaMax{"delEtaMax", +1.0, "delEtaMax"};

	Configurable<float> collisionZCut{"collisionZCut", 10.0f, "collisionZCut"};

	// Histogram registry
	HistogramRegistry histos{"histos", {}};

	// Define table alias to consume
	using FilteredDrCollisions = soa::Filtered<aod::DrCollisions>;
	using FilteredDrCollision  = FilteredDrCollisions::iterator;
	Filter CollZFilter = nabs(aod::collision::posZ) < collisionZCut;

	Partition<aod::DrTracks> AssoTracks = (aod::exampleTrackSpace::pt > 4.0f) and (aod::exampleTrackSpace::pt < 6.0f);
	Partition<aod::DrTracks> TrigTracks = (aod::exampleTrackSpace::pt >= 6.0f);

	// Initialization
	void init(InitContext const& cfgc)
	{
	
		AxisSpec axisCollisionZ{nBinCollisionZ, collisionZMin, collisionZMax, "Z(cm)"};
		AxisSpec axisPt{nBinPt, ptMin, ptMax, "p_{T}(GeV/c)"};
		AxisSpec axisDelPhi{nBinDelPhi, delPhiMin, delPhiMax, "#Delta#Phi"};
		AxisSpec axisDelEta{nBinDelEta, delEtaMin, delEtaMax, "#Delta#eta"};

		histos.add("correlationFunction", "correlationFunction", kTH1F, {axisDelPhi});
		histos.add("h2CorrelationFunction", "h2CorrelationFunction", kTH2F, {axisDelPhi, axisDelEta});

		histos.add("collisionZ", "collisionZ", kTH1F, {axisCollisionZ});
		histos.add("pTAsso1", "pTAsso1", kTH1F, {axisPt});
		histos.add("pTAsso2", "pTAsso2", kTH1F, {axisPt});
		histos.add("pTTrig", "pTTrig", kTH1F, {axisPt});
	}

	// Process function
	void process(FilteredDrCollision const& collision, aod::DrTracks const& tracks)
	{
		// Manually connect partioned table to corresponding collision idx
		auto AssoTracksThisCollision = AssoTracks -> sliceByCached( aod::exampleTrackSpace::drCollisionId, collision.globalIndex(), cache);
		auto TrigTracksThisCollision = TrigTracks -> sliceByCached( aod::exampleTrackSpace::drCollisionId, collision.globalIndex(), cache);
		
		LOGP(info, "Collision global index : {}", collision.globalIndex());

		// Make correlation
		for( auto const& trig : TrigTracksThisCollision )
		{
			LOGP(info, "Filled Trig paticle's global index : {}", trig.globalIndex());
			for( auto const& asso : AssoTracksThisCollision )
			{
				LOGP(info, "Filled asso1 paticle's global index : {}", asso.globalIndex());
				histos.fill(HIST("pTAsso1"), asso.pt()); // -> Filling this histogram here returns differnet resutl...unexpected result
														 // -> If there is no Trigger particles found, then no filling activated
														 // -> Over counting of associated particles, since assoc loops hapens for all triger particles inside the event
														 // -> The entry number of this histogram is same with entry number of h2CorrelationFunction
				// Calculation of DeltaPhi
				double deltaPhi = trig.phi() - asso.phi();
				double deltaEta = trig.eta() - asso.eta();

				if( deltaPhi < -TMath::Pi()/2 ){	
					deltaPhi += 2*TMath::Pi();
				}else if( deltaPhi > 3*TMath::Pi()/2 ){
					deltaPhi -= 2*TMath::Pi();
				}

				histos.fill(HIST("correlationFunction"), deltaPhi);
				histos.fill(HIST("h2CorrelationFunction"), deltaPhi, deltaEta);
			}
		}

		//!Fill in QA histos
		histos.fill(HIST("collisionZ"), collision.posZ());

		for( auto const& track : AssoTracksThisCollision )
		{
			LOGP(info, "Filled asso2 paticle's global index : {}", track.globalIndex());
			histos.fill(HIST("pTAsso2"), track.pt()); // -> Filling histogram here prevents overcounting and counts associate particles in events without trig particle	
		}

		for( auto const& track : TrigTracksThisCollision )
		{
			histos.fill(HIST("pTTrig"), track.pt());	
		}
	}

};

// Declare partition inside process loop
// Use .bindTable
struct handson2ver1{
	// Declaing configurable
	Configurable<int>	nBinCollisionZ{"nBinCollisionZ", 400, "nBinCollisionZ"};
	Configurable<float> collisionZMin{"collisionZMin", -20, "collisionZMin"};
	Configurable<float> collisionZMax{"collisionZMax", +20, "collisionZMax"};
	
	Configurable<int>	nBinPt{"nBinPt", 400, "nBinPt"};
	Configurable<float>	ptMin{"ptMin", 0, "ptMin"};
	Configurable<float>	ptMax{"ptMax", 0, "ptMax"};

	Configurable<int> nBinDelPhi{"nBinDelPhi", 100, "nBinDelPhi"};
	Configurable<float> delPhiMin{"delPhiMin", -0.5*TMath::Pi(), "delPhiMin"};
	Configurable<float> delPhiMax{"delPhiMax", +1.5*TMath::Pi(), "delPhiMax"};

	Configurable<int> nBinDelEta{"nBinDelEta", 100, "nBinDelEta"};
	Configurable<float> delEtaMin{"delEtaMin", -1.0, "delEtaMin"};
	Configurable<float> delEtaMax{"delEtaMax", +1.0, "delEtaMax"};

	Configurable<float> collisionZCut{"collisionZCut", 10.0f, "collisionZCut"};

	// Histogram registry
	HistogramRegistry histos{"histos", {}};

	// Define table alias to consume
	using FilteredDrCollisions = soa::Filtered<aod::DrCollisions>;
	using FilteredDrCollision  = FilteredDrCollisions::iterator;
	Filter CollZFilter = nabs(aod::collision::posZ) < collisionZCut;

	// Initialization
	void init(InitContext const& cfgc)
	{
	
		AxisSpec axisCollisionZ{nBinCollisionZ, collisionZMin, collisionZMax, "Z(cm)"};
		AxisSpec axisPt{nBinPt, ptMin, ptMax, "p_{T}(GeV/c)"};
		AxisSpec axisDelPhi{nBinDelPhi, delPhiMin, delPhiMax, "#Delta#Phi"};
		AxisSpec axisDelEta{nBinDelEta, delEtaMin, delEtaMax, "#Delta#eta"};

		histos.add("correlationFunction", "correlationFunction", kTH1F, {axisDelPhi});
		histos.add("h2CorrelationFunction", "h2CorrelationFunction", kTH2F, {axisDelPhi, axisDelEta});

		histos.add("collisionZ", "collisionZ", kTH1F, {axisCollisionZ});
		histos.add("pTAsso", "pTAsso", kTH1F, {axisPt});
		histos.add("pTTrig", "pTTrig", kTH1F, {axisPt});
	}

	// Process
	void process( FilteredDrCollisions const& collisions, aod::DrTracks const& drTracks)
	{
		for( auto const& collision : collisions)
		{
			// Define Partition
			Partition<aod::DrTracks> AssoTracksThisCollision
				= (aod::exampleTrackSpace::drCollisionId == collision.globalIndex()) and ((aod::exampleTrackSpace::pt > 4.0f) and (aod::exampleTrackSpace::pt < 6.0f));

			Partition<aod::DrTracks> TrigTracksThisCollision
				= (aod::exampleTrackSpace::drCollisionId == collision.globalIndex()) and (aod::exampleTrackSpace::pt >= 6.0f);

			// Fill Partition with .bindTable()
			AssoTracksThisCollision.bindTable(drTracks);
			TrigTracksThisCollision.bindTable(drTracks);

			histos.get<TH1>(HIST("collisionZ")) -> Fill(collision.posZ());

			// Fill in correlation
			for( auto const& TrigTrack : TrigTracksThisCollision )
			{
				for( auto const& AssoTrack : AssoTracksThisCollision )
				{
					//histos.get<TH1>(HIST("pTAsso")) -> Fill(AssoTrack.pt());

					double deltaEta = TrigTrack.eta() - AssoTrack.eta();
					double deltaPhi = TrigTrack.phi() - AssoTrack.phi();

					if( deltaPhi < -TMath::Pi()/2 ){	
						deltaPhi += 2*TMath::Pi();
					} else if( deltaPhi > 3*TMath::Pi()/2 ){
						deltaPhi -= 2*TMath::Pi();
					}
					
					histos.get<TH2>(HIST("h2CorrelationFunction")) -> Fill(deltaPhi, deltaEta);
					histos.get<TH1>(HIST("correlationFunction")) -> Fill(deltaPhi);
				}
			}

			// QA
			for( auto const& TrigTrack : TrigTracksThisCollision )
			{
				histos.get<TH1>(HIST("pTTrig")) -> Fill(TrigTrack.pt());
			}

			for( auto const& AssoTrack : AssoTracksThisCollision )
			{
				histos.get<TH1>(HIST("pTAsso")) -> Fill(AssoTrack.pt());
			}

		} // collision loop
	}
};

struct handson2ver2{

	// Configurables
	Configurable<int>	nBinCollisionZ{"nBinCollisionZ", 400, "nBinCollisionZ"};
	Configurable<float> collisionZMin{"collisionZMin", -20, "collisionZMin"};
	Configurable<float> collisionZMax{"collisionZMax", +20, "collisionZMax"};
	
	Configurable<int>	nBinPt{"nBinPt", 400, "nBinPt"};
	Configurable<float>	ptMin{"ptMin", 0, "ptMin"};
	Configurable<float>	ptMax{"ptMax", 0, "ptMax"};

	Configurable<int> nBinDelPhi{"nBinDelPhi", 100, "nBinDelPhi"};
	Configurable<float> delPhiMin{"delPhiMin", -0.5*TMath::Pi(), "delPhiMin"};
	Configurable<float> delPhiMax{"delPhiMax", +1.5*TMath::Pi(), "delPhiMax"};

	Configurable<int> nBinDelEta{"nBinDelEta", 100, "nBinDelEta"};
	Configurable<float> delEtaMin{"delEtaMin", -1.0, "delEtaMin"};
	Configurable<float> delEtaMax{"delEtaMax", +1.0, "delEtaMax"};

	Configurable<float> collisionZCut{"collisionZCut", 10.0f, "collisionZCut"};
	
	// Histogram registry
	HistogramRegistry histos{"histos", {}};

	// Init
	void init(InitContext const& cfgc)
	{
		AxisSpec axisCollisionZ{nBinCollisionZ, collisionZMin, collisionZMax, "Z(cm)"};
		AxisSpec axisPt{nBinPt, ptMin, ptMax, "p_{T}(GeV/c)"};
		AxisSpec axisDelPhi{nBinDelPhi, delPhiMin, delPhiMax, "#Delta#phi"};
		AxisSpec axisDelEta{nBinDelEta, delEtaMin, delEtaMax, "#Delta#eta"};

		histos.add("collisionZ", "collisionZ", kTH1F, {{axisCollisionZ}});
		histos.add("ptAsso", "ptAsso", kTH1F, {{axisPt}});
		histos.add("ptTrig", "ptTrig", kTH1F, {{axisPt}});

		histos.add("corrfunc", "corrfunc", kTH1F, {axisDelPhi});
		histos.add("h2CorrFunc", "h2CorrFunc", kTH2F, {{axisDelPhi, axisDelEta}});
	}
	
	// Defining table alias
	using FilteredDrCollisions = soa::Filtered<aod::DrCollisions>;
	Filter posZFilter = nabs(aod::collision::posZ) < 10.0f;
	
	Preslice<aod::DrTracks> drTracksPerCollision = aod::exampleTrackSpace::drCollisionId;

	// process function
	void process( FilteredDrCollisions const& collisions, aod::DrTracks const& tracks )
	{
		for( auto const& collision : collisions)
		{
			// Make preslice -> Only gropuing is possible?
			auto TracksPerCollisionSliced= tracks.sliceBy( drTracksPerCollision, collision.globalIndex());
			Partition<aod::DrTracks> AssoTracks = (aod::exampleTrackSpace::pt > 4.0f) and (aod::exampleTrackSpace::pt < 6.0f);
			Partition<aod::DrTracks> TrigTracks = (aod::exampleTrackSpace::pt >= 6.0f);

			// Bind tables
			AssoTracks.bindTable(TracksPerCollisionSliced);
			TrigTracks.bindTable(TracksPerCollisionSliced);

			histos.fill(HIST("collisionZ"), collision.posZ());

			// Correlation
			for( auto const& trig : TrigTracks )
			{
				for( auto const& asso : AssoTracks )
				{
					double delEta = trig.eta() - asso.eta();
					double delPhi = trig.phi() - asso.phi();

					// Re-arrange delta phi 
					if( delPhi > 3*TMath::Pi()/2 ) {
						delPhi -= 2*TMath::Pi();
					} else if( delPhi < -TMath::Pi()/2 ) {
						delPhi += 2*TMath::Pi();
					}

					histos.fill(HIST("corrfunc"), delPhi);
					histos.fill(HIST("h2CorrFunc"), delPhi, delEta);
				}
			}

			// QA
			for( auto const& trig : TrigTracks )
			{
				histos.fill(HIST("ptTrig"), trig.pt());
			}

			for( auto const& asso : AssoTracks )
			{
				histos.fill(HIST("ptAsso"), asso.pt());
			}

		} // collision loop
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<handson2ver0>(cfgc),
		adaptAnalysisTask<handson2ver1>(cfgc),
		adaptAnalysisTask<handson2ver2>(cfgc),
	};
}

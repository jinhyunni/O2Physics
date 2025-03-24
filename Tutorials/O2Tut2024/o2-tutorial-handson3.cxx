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
struct handson3ver0{

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
		histos.add("pTAsso", "pTAsso", kTH1F, {axisPt});
		histos.add("pTTrig", "pTTrig", kTH1F, {axisPt});
	}

	// Process function
	void process(FilteredDrCollision const& collision, aod::DrTracks const& tracks)
	{
		// Manually connect partioned table to corresponding collision idx
		auto AssoTracksThisCollision = AssoTracks -> sliceByCached( aod::exampleTrackSpace::drCollisionId, collision.globalIndex(), cache);
		auto TrigTracksThisCollision = TrigTracks -> sliceByCached( aod::exampleTrackSpace::drCollisionId, collision.globalIndex(), cache);

		// Make correlation
		for( auto const& trig : TrigTracksThisCollision )
		{
			for( auto const& asso : AssoTracksThisCollision )
			{
				histos.fill(HIST("pTAsso"), asso.pt()); // -> Filling this histogram here returns differnet resutl...unexpected result
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
			//histos.fill(HIST("pTAsso"), track.pt());	
		}

		for( auto const& track : TrigTracksThisCollision )
		{
			histos.fill(HIST("pTTrig"), track.pt());	
		}
	}

};

// Declare partition inside process loop
// Use .bindTable
struct handson3ver1{
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
					histos.get<TH1>(HIST("pTAsso")) -> Fill(AssoTrack.pt()); // -> Filling this histogram at seperate loop returns different filling

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
				//histos.get<TH1>(HIST("pTAsso")) -> Fill(AssoTrack.pt());
			}

		} // collision loop
	}
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<handson3ver0>(cfgc),
		adaptAnalysisTask<handson3ver1>(cfgc),
	};
}

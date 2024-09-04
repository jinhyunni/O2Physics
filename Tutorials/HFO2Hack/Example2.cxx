// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
///
/// \brief Consume task processed intermediate tables created by Preprocess task
/// \author
/// \since

#include "Framework/runDataProcessing.h"
#include "Framework/AnalysisTask.h"

using namespace o2;
using namespace o2::framework;

struct basicrootstyle
{
	//Prepare Histogram in root style
	TH1F* hist_root = new TH1F("pt_root", "pt_root", 100, 0., 10.);
	
	//Filling histogram
	void process(aod::Tracks const& tracks)
	{
		for(auto const& track: tracks)
		{
			hist_root -> Fill( track.pt() );
		}
	}

};

struct usinghistregistrywithobject
{
	// To add a histogram, first include histogmam registry
	//HistogramRegistry histos{"histos", {}, OutputObjHandlingPolicy::AnalysisObject};
	HistogramRegistry histos{"histos", {} };
	
	// Axis configurabls for pT
	Configurable<int> pTBinN{"pTBinN", 100, "pTBinN"};
	Configurable<float> pTMin{"pTMin", 0., "pTMin"};
	Configurable<float> pTMax{"pTMax", 10., "pTMax"};

	// Axis configurables for eta
	Configurable<int> etaBinN{"etaBinN", 120, "etaBinN"};
	Configurable<float> etaMin{"etaMin", -6., "etaMin"};
	Configurable<float> etaMax{"etaMax", 6., "etaMax"};

	// Initialize
	void init(InitContext const&)
	{
		// Define axis
		const AxisSpec axisPt{pTBinN, pTMin, pTMax, "p_{T}"};
		const AxisSpec axisEta{etaBinN, etaMin, etaMax, "#eta"};

		// Create and add histogram to registry
		histos.add("pT", "pT", kTH1F, {axisPt});
		histos.add("eta","eta", kTH1F, {axisEta});
	}
		
	// Subscribed to Tracks table
	void process(aod::Tracks const& tracks)
	{	
		//Looping over Tracks table!
		for(auto const& track : tracks)
		{
			histos.fill( HIST("pT"), track.pt() );
			histos.fill( HIST("eta"), track.eta() );
		}
	}
};

struct usinghistregistrywithpointer
{
	Configurable<int> nBins{"nBins", 100, "nBins"};
	Configurable<float> nStart{"start", 0., "start"};
	Configurable<float> nFinish{"finish", 10., "finish"};

	HistogramRegistry registry{

		"histos", 
		{
			{"pT_UsingHistPtr", "pT",{ HistType::kTH1F, {{nBins, nStart, nFinish, "p_{T}(GeV/c2)"}}}} // HistSpec1
		}
	};
	
	void process(aod::Tracks const& tracks)
	{
		for(auto const& track : tracks)
		{
			registry.get<TH1>(HIST("pT_UsingHistPtr")) -> Fill( track.pt() );
		}
	}
};

struct usinghistfactory
{
	// Axis information declared as Configurable<type>
	Configurable<int> nBins{"nBins", 1000, "nBins" };
	Configurable<float> nMax{"Max", 10., "Max"};
	Configurable<float> nMin{"Min", 0., "Min"};
	
	// Define AxisSpec, HistogramSpec
	AxisSpec axispT{nBins, nMin, nMax}; // single AxisSpec
	HistogramConfigSpec histConfigSpec{HistType::kTH1F, {axispT}};
 	HistogramSpec pTHistSpec{"pT_UsingHistFactory", "pT", histConfigSpec };

	// Making a histogram with HistFactory
	//HistPtr pTHist = HistFactory::createHist( pTHistSpec );
	//std::shared_ptr<TH1> pTHist = HistFactory::createHist( const& pTHistSpec );
	HistPtr pTHist = HistFactory::createHistVariant( pTHistSpec );
	
	TH1* hist = std::get< std::shared_ptr<TH1> >( pTHist ).get();

	void process(aod::Tracks const& tracks)
	{

		for( auto const& track: tracks)
		{
			hist -> Fill(track.pt());
		}
	}
	
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{ 
		adaptAnalysisTask<basicrootstyle>(cfgc),
		adaptAnalysisTask<usinghistregistrywithobject>(cfgc),
		adaptAnalysisTask<usinghistregistrywithpointer>(cfgc),
		adaptAnalysisTask<usinghistfactory>(cfgc),
	};
}

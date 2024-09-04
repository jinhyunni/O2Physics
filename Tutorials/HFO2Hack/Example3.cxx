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

// Grouping, tracks per collision?
// Calculating average pT per collision!
struct trackperevent1{
	
	HistogramRegistry registry{"histos", {}};

	Configurable<int> nBins{"nBins", 100, "nBins"};
	Configurable<float> nMin{"Min", 0., "Min"};
	Configurable<float> nMax{"Max", 10., "Max"};
	Configurable<int> printRate{"printRate", 50, "printRate"};

	void init(InitContext const&)
	{
		AxisSpec pTAxis{nBins, nMin, nMax};
		registry.add("avgpT", "avgpT", HistType::kTH1F, {pTAxis});
		registry.add("pT", "pT", HistType::kTH1F, {pTAxis});
	}

	// Subscribing to collision iterator and track table!
	void process(aod::Collision const& collision, aod::Tracks const& tracks)
	{
		float avgpt = 0.;

		for(auto const& track: tracks)
		{
			avgpt += track.pt();
			registry.fill( HIST("pT"), track.pt() );
		}

		// Get the size of tracks table! -> # of tracks per collision!
		// .size() -> std method in vector? -> Check how table rows are contained...

		if( tracks.size() != 0)
		{
			avgpt /= tracks.size();
			
			if(collision.globalIndex() % printRate == 0)
			{
				LOGP(info, "In collision{}, track#: {} AvgpT: {}", collision.globalIndex(), tracks.size(), avgpt);
			}

			//Fill average pT to histogram
			registry.fill( HIST("avgpT"), avgpt);
		}
	}
	
};

// Creating event counter...
struct trackperevent2
{
	// Define histogram registry
	HistogramRegistry registry{"histos", {}};

	// Configurables for pT histogram, event counter
	Configurable<int> nBins{"nBins", 100, "nBins"};
	Configurable<float> min{"min", 0., "min"};
	Configurable<float> max{"max", 10., "max"};

	void init(InitContext const&)
	{
		// Define Histogram axis
		AxisSpec pTAxis{nBins, min, max};
		AxisSpec eventCounterAxis{1, 0, 1};
		AxisSpec trackCounterAxis{1, 0, 1};

		//Define histogram
		registry.add("pT", "pT", {HistType::kTH1F,{pTAxis}});
		registry.add("event", "event", {HistType::kTH1F,{eventCounterAxis}});
		registry.add("trackNum", "trackNum", {HistType::kTH1F, {trackCounterAxis}});
	}

	// Subscribe to Collisions table's iterator, and Tracks table


	void process(aod::Collision const& collision, aod::Tracks const& tracks)
	{
		registry.fill( HIST("event"), 0.5);
		
		for(auto const& track : tracks)
		{
			registry.fill( HIST("pT"), track.pt() );
			registry.fill( HIST("trackNum"), 0.5 );
		}
	}
};


struct trackperdataframe1{
	
	//Make Histogram registry
 //*	HistogramRegistry registry{
 //*		"Histos",
 //*		{ "avgpT", "avgpT", HistType::kTH1F, {{20, 0., 2.}} }
 //*	};

	HistogramRegistry registry{"histos", {}};

	Configurable<int> nBins{"nBins", 100, "nBins"};
	Configurable<float> min{"min", 0., "min"};
	Configurable<float> max{"max", 10., "max"};
	
	void init(InitContext const&)
	{
		AxisSpec pTAxis{nBins, min, max};
		registry.add("avgpT", "avgpT", kTH1F, {pTAxis});
		registry.add("pT", "pT", kTH1F, {pTAxis});
	}

	// per data frame?
	void process(aod::Collisions const& collisions , aod::Tracks const& tracks)
	{
		float avgpt = 0.;
		int numtracks = 0.;
		//calculating sum of track pT per dataframe
		for(auto const& track: tracks)
		{
			registry.fill(HIST("pT"), track.pt());
			avgpt += track.pt();
			numtracks++;
		}

		//divide by # of tracks
		avgpt /= numtracks;

		//Fill avgpT to histogram
		registry.fill(HIST("avgpT"), avgpt);
		
	}

};

struct trackperdataframe2{
	
	// Define Histogram registry
	HistogramRegistry registry{
	
		"histos",
		// vector<HistogramSpec>
		{
			// First HistogramSpec vector element
			{
				"avgpT", "avgpT", 
				// HistogramConfigSpec
				{ 
					HistType::kTH1F, 
					// vector<AxisSpec>: for multidimensional histogram
					{
						{20, 0., 2.}
					}
				}
			}
		}
	};
	
	//Only subscribe to Tracks table
	void process(aod::Tracks const& tracks)
	{
		float avgpt = 0;
		int ntracks =0;
		for(auto const& track : tracks)
		{
			avgpt += track.pt();
			ntracks++;
		}
		
		//making average pT per dataframe
		avgpt /= ntracks;

		registry.fill( HIST("avgpT"), avgpt );
	}

};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<trackperevent1>(cfgc),
		adaptAnalysisTask<trackperevent2>(cfgc),
		adaptAnalysisTask<trackperdataframe1>(cfgc),
		adaptAnalysisTask<trackperdataframe2>(cfgc),
	};
}

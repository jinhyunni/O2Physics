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

constexpr int each = 1e5;

struct Example1
{
	// To add a histogram, first include histogmam registry

	const int binwidth = 0.1;
	const float start = 0.;
	const float finish =10.;
	const int nBins = (finish - start)/binwidth;

	// Define Histogram registry
	HistogramRegistry hist
	{
	  //{"name", "title", {HistType::kTH1~, {nBins, start, finish}} };
		{"pT", "pT", {HistType::kTH1F, {nBins, start, finish}} };
	}
	
	// Make a histogram pointer
	auto* hist_ptr = hist.get<TH1>( HIST("pT") );
	
	// Subscribed to Tracks table
	void process(aod::Tracks const& tracks)
	{	
		//Looping over Tracks table!
		for(auto const& track : tracks)
		{
			hist_ptr -> Fill( track.pt() );
		}
	}
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec{ adaptAnalysisTask<Example1>(cfgc) };
}

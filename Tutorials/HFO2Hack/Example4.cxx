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
struct example4{
	
	HistogramRegistry registry{"histos", {}};

	Configurable<int> nBins{"nBins", 101, "nBins"};
	Configurable<float> nMin{"Min", -0.05, "Min"};
	Configurable<float> nMax{"Max", 10.05, "Max"};

	void init(InitContext const&)
	{
		AxisSpec hpt{nBins, nMin, nMax};
		registry.add("pT", "pT", HistType::kTH1F, {hpt});
	}



	// Subscribing to collision iterator and track table!
	void process()
	{

	}
	
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<example4>(cfgc),
	};
}

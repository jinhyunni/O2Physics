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
//#include "Common/DataModel/TrackSelectionTables.h"

using namespace o2;
using namespace o2::framework;

constexpr int each = 1e5;

struct Example1
{
	void process(aod::Tracks const& tracks)
	{
		for(auto const& track : tracks)
		{
			if(track.index() % each == 0)
			{
				LOGP(info, "Track {} has pT = {}", track.index(), track.pt());
			}
		}// Loop over table
	}//subscription to table
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec{ adaptAnalysisTask<Example1>(cfgc) };
}

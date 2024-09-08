// Example7: Making Index table and using them
// Objective: ???

#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Common/DataModel/Multiplicity.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct Example7{
	
	void init(InitContext&){}

	void process(){}
}

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
	return WorkflowSpec
	{
		adaptAnalysisTask<Example7>(cfgc)
	};
}

#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"
OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/MC/AO2D.root"
CONFIG="-b --configuration json://dpl-config-ex9-ref1.json"

o2-hfo2hacktut-example9-ref1 ${CONFIG} | \
	
	# Running Converters for updating old version of tables
#o2-analysis-bc-converter ${CONFIG} | \
	o2-analysis-mccollision-converter ${CONFIG} | \

	# Running analysis	
	o2-analysis-track-propagation ${CONFIG} | \
	o2-analysis-tracks-extra-v002-converter ${CONFIG} | \
	o2-analysis-event-selection ${CONFIG} | \
	o2-analysis-multiplicity-table ${CONFIG} | \
	o2-analysis-timestamp ${CONFIG} 

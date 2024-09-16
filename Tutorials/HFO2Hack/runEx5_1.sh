OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day2/AO2D-mc.root"
CONFIG="-b --configuration json://dpl-config-ex5_1.json"

# Main analysis
o2-hfo2hacktut-example5sub1 ${CONFIG} | \
	
	# Running converters
	o2-analysis-bc-converter ${CONFIG} | \
	o2-analysis-mccollision-converter ${CONFIG} | \
	
	# Running helper task
	o2-analysis-multiplicity-table ${CONFIG} | \
	o2-analysis-track-propagation ${CONFIG} | \
	o2-analysis-timestamp ${CONFIG} | \
	o2-analysis-tracks-extra-converter ${CONFIG} | \
	o2-analysis-event-selection ${CONFIG}

#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/AO2D.root"
OPTION="-b --configuration json://dpl-config-ex5.json"

o2-hfo2hacktut-example5 ${OPTION} | \
	o2-analysis-track-propagation ${OPTION} | \
	o2-analysis-timestamp ${OPTION} | \
	o2-analysis-event-selection ${OPTION} | \
	o2-analysis-tracks-extra-v002-converter ${OPTION} | \
	o2-analysis-multiplicity-table ${OPTION} 

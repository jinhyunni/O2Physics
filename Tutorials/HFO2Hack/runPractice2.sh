#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"
OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day2/AO2D-mc.root"
CONFIG="-b --configuration json://dpl-config-prac2.json"

o2-hfo2hacktut-practice2 ${OPTION} | \
	
 	# Running Converters for updating old version of tables
 	o2-analysis-bc-converter ${OPTION} | \
	o2-analysis-mccollision-converter ${OPTION} | \

	# Running analysis?	
	o2-analysis-track-propagation ${OPTION} | \
	o2-analysis-timestamp ${OPTION} | \
	o2-analysis-tracks-extra-converter ${OPTION}

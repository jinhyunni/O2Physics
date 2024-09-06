#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"
OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day2/AO2D-mc.root"
CONFIG="-b --configuration json://dpl-config-ex4.json"

o2-hfo2hacktut-example4 ${CONFIG} | \
	
	# Running Converters for updating old version of tables
	o2-analysis-bc-converter ${CONFIG} | \
	o2-analysis-mccollision-converter ${CONFIG} | \

	# Running analysis?	
	o2-analysis-track-propagation ${CONFIG} | \
	o2-analysis-timestamp ${CONFIG} | \
	o2-analysis-tracks-extra-converter ${CONFIG}

 #o2-hfo2hacktut-example4 -b --configuration=json://dpl-config-ex4.json | \
 #o2-analysis-track-propagation -b --configuration=json://dpl-config-ex4.json | \
 #o2-analysis-timestamp -b --configuration=json://dpl-config-ex4.json | \
 #o2-analysis-tracks-extra-converter -b --configuration=json://dpl-config-ex4.json

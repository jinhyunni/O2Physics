#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"
OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day2/AO2D-mc.root"
CONFIG="-b --configuration json://dpl-config-ex7.json"
WRITETTREE="-b --aod-writer-keep AOD/MultsGen/0,AOD/Tracks/0,AOD/McParticles/0 --configuration json://dpl-config-ex7.json"

o2-hfo2hacktut-example7 ${WRITETTREE} | \
	
	# Running Converters for updating old version of tables
	o2-analysis-bc-converter ${WRITETTREE} | \
	o2-analysis-mccollision-converter ${WRITETTREE} | \

	# Running analysis	
	o2-analysis-track-propagation ${WRITETTREE} | \
	o2-analysis-tracks-extra-converter ${WRITETTREE} | \
	o2-analysis-event-selection ${WRITETTREE} | \
	o2-analysis-timestamp ${WRITETTREE} | \
	o2-analysis-multiplicity-table ${WRITETTREE}

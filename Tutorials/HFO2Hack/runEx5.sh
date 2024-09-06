OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"
CONFIG='-b --configuration json://dpl-config-ex5.json'

 ##o2-analysis-multiplicity-table ${OPTION} | \
 #o2-analysis-track-propagation ${OPTION} | \
 #o2-analysis-timestamp ${OPTION} | \
 #o2-analysis-tracks-extra-converter ${OPTION} | \
 #
 #o2-analysis-multiplicity-table ${OPTION} | \

# Executable for running example5
o2-hfo2hacktut-example5 ${CONFIG} | \
		o2-analysis-track-propagation ${CONFIG} | \
		o2-analysis-tracks-extra-converter ${CONFIG} | \
		o2-analysis-event-selection ${CONFIG} | \
		o2-analysis-multiplicity-table ${CONFIG} | \
		o2-analysis-timestamp ${CONFIG}

OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"

o2-analysis-multiplicity-table ${OPTION} | \
o2-analysis-track-propagation ${OPTION} | \
o2-analysis-timestamp ${OPTION} | \
o2-analysis-tracks-extra-converter ${OPTION} | \
o2-hfo2hacktut-example5sub1 ${OPTION}

#o2-hfo2hacktut-example4 --configuration=json://dpl-config-ex4.json

OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"

o2-hfo2hacktut-example3 ${OPTION} | \
o2-analysis-track-propagation ${OPTION} | \
o2-analysis-timestamp ${OPTION} | \
o2-analysis-tracks-extra-converter ${OPTION}

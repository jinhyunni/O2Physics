#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/AO2D.root"
OPTION="-b --configuration json://dpl-config-ex3.json"

o2-hfo2hacktut-example3 ${OPTION} | \
o2-analysis-track-propagation ${OPTION} | \
o2-analysis-timestamp ${OPTION} 
#o2-analysis-tracks-extra-converter ${OPTION}

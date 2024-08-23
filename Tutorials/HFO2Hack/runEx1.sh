OPTION="-b configuration json://dpl-config.json"

o2-hfo2hacktut-example1 ${OPTION} | \
o2-analysis-track-propagation ${OPTION}
#o2-analysis-timestamp ${OPTION}
#o2-analysis-tracks-extra-converter ${OPTION}

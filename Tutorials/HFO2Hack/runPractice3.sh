OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day1_Introduction/AO2D.root"
#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day2/AO2D-mc.root"
CONFIG="-b --configuration json://dpl-config-prac3-day1.json"

o2-hfo2hacktut-practice3 ${CONFIG} | \
	
 	# Running Converters for updating old version of tables
#o2-analysis-bc-converter ${CONFIG} | \
#o2-analysis-mccollision-converter ${CONFIG} | \

	# Running analysis - Helper task for making tracks table
	o2-analysis-track-propagation ${CONFIG} | \
	o2-analysis-timestamp ${CONFIG} | \
	o2-analysis-tracks-extra-converter ${CONFIG} | \
	
	# Running analysis - Helper task for making PID TOF & TPC
	o2-analysis-event-selection ${CONFIG} | \
	o2-analysis-trackselection ${CONFIG} | \
	o2-analysis-multiplicity-table ${CONFIG} | \
	o2-analysis-pid-tpc ${CONFIG} | \
	o2-analysis-pid-tpc-qa ${CONFIG} | \
	o2-analysis-pid-tpc-base ${CONFIG} | \
	o2-analysis-pid-tof ${CONFIG} | \
	o2-analysis-pid-tof-beta ${CONFIG} | \
	o2-analysis-pid-tof-qa ${CONFIG} | \
	o2-analysis-pid-tof-base ${CONFIG}

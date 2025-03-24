#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial3.0/day2/AO2D-mc.root"
#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/HFTut/AO2D.root"
#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/MC/AO2D.root"
OPTION="-b --configuration json://dpl-config-ex7.json"
LOGFILE="stdout-ex7.log"

o2-hfo2hacktut-example7 ${OPTION} | \
	o2-analysis-track-propagation ${OPTION} | \
	o2-analysis-timestamp ${OPTION} | \
	o2-analysis-event-selection ${OPTION} | \
	o2-analysis-tracks-extra-v002-converter ${OPTION} | \
	o2-analysis-mccollision-converter ${OPTION} \
	> "$LOGFILE" 2>&1

rc=$?

if [ $rc -eq 0 ] ; then
	echo "No problems!"
else
	echo "Error: Exit code $rc"
	echo "Check the log file: ${LOGFILE}"
	exit $rc
fi

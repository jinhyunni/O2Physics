#OPTION="-b --aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/AO2D.root"
OPTION="-b --configuration json://dpl-config-ex6.json --aod-memory-rate-limit 2000000000 --shm-segment-size 16000000000 --resources-monitoring 2"
LOGFILE="stdout-ex6.log"

o2-hfo2hacktut-example6 ${OPTION} | \
	o2-analysis-track-propagation ${OPTION} | \
	o2-analysis-timestamp ${OPTION} | \
	o2-analysis-event-selection ${OPTION} | \
	o2-analysis-tracks-extra-v002-converter ${OPTION} | \
	o2-analysis-multiplicity-table ${OPTION} \
	> "$LOGFILE" 2>&1

rc=$?

if [ $rc -eq 0 ] ; then
	echo "No problems!"
else
	echo "Error: Exit code $rc"
	echo "Check the log file: ${LOGFILE}"
	exit $rc
fi

OPTIONS=(
		-b 
		--aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/MC/AO2D.root
#--configuration json://dpl-config-handson3.json
#--aod-memory-rate-limit 2000000000
#--shm-segment-size 16000000000
#--resource-monitoring 2
		)
LOGFILE="stdout-handson3.log"

o2-tutorial2024-tutorial3 "${OPTIONS[@]}" | \
	o2-analysis-timestamp "${OPTIONS[@]}" | \
	o2-analysis-track-propagation "${OPTIONS[@]}" | \
	o2-analysis-tracks-extra-v002-converter "${OPTIONS[@]}" \
#> "${LOGFILE}" 2>&1

rc=$?

if [ $rc -eq 0 ] ; then
	echo "No problems!"
else
	echo "Error : Exit code $rc"
	echo "Check the log file : ${LOGFILE}"
	exit $rc
fi

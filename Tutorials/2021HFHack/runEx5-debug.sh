#!/bin/bash

RUNSUFFIX="example5"

LOGSUFFIX="stdout-${RUNSUFFIX}.log"
JSON="dpl-config-${RUNSUFFIX}.json"

#DIR_THIS="$(dirname "$(realpath "$0")")"
#JSON="${DIR_THIS}/dpl-config-ex5-debug.json"
#JSON="dpl-config-ex5-debug.json"

OPTIONS=(
		-b 
		--aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/O2Tutorial4.0/AO2D.root
		#--configuration json://$JSON
		--aod-writer-json ${JSON}
)

o2-hfo2hacktut-example5 "${OPTIONS[@]}" | \
	o2-analysis-track-propagation "${OPTIONS[@]}" | \
	o2-analysis-tracks-extra-v002-converter "${OPTIONS[@]}" | \
	o2-analysis-timestamp "${OPTIONS[@]}" | \
	o2-analysis-multiplicity-table "${OPTIONS[@]}" | \
	o2-analysis-event-selection "${OPTIONS[@]}" \
> "$LOGFILE" 2>&1
	
rc=$?
if [ $rc -eq 0 ]; then
	echo "No problems!"
else
	echo "ERROR: Exit code $rc"
	echo "Check the log file $LOGFILE"
	exit $rc
fi
	

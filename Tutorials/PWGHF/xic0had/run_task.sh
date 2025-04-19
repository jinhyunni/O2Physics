#!/bin/bash

# log file where the terminal output will be saved
LOGFILE="stdout.log"

# directory of this script
DIR_THIS="$(dirname "$(realpath "$0")")"

# O2 configuration file (in the same directory)
#JSON="$DIR_THIS/dpl-config.json"
JSON="$DIR_THIS/config_ppref_mar2025.json"

# command line options of O2 workflows
OPTIONS=(
  #-b
  #--aod-file /Users/jinhyunpark/npl/Analysis/ALICE/data/ao2d/pp5036GeV/data/hyperloop_DST_ppref_dataset_559348.root
  --configuration json://"$JSON"
  --aod-writer-json OutputDirector.json
)

# Run analysis
# o2-analysis-lf-lambdakzerobuilder have to be replaced with o2-analysis-lf-strangenessbuilder( Announcement at Mattermost 2025.4.17 )
#o2-analysis-hf-tree-creator-to-xi-pi "${OPTIONS[@]}" | \
#o2-analysis-hf-candidate-creator-xic0-omegac0 "${OPTIONS[@]}" | \
#o2-analysis-hf-candidate-selector-to-xi-pi "${OPTIONS[@]}" | \
o2-analysis-timestamp "${OPTIONS[@]}" | \
o2-analysis-lf-lambdakzerobuilder "${OPTIONS[@]}" | \
o2-analysis-pid-tof-full "${OPTIONS[@]}" | \
o2-analysis-pid-tof-base "${OPTIONS[@]}" | \
o2-analysis-lf-cascadebuilder "${OPTIONS[@]}" | \
o2-analysis-ft0-corrected-table "${OPTIONS[@]}" | \
o2-analysis-event-selection "${OPTIONS[@]}" | \
o2-analysis-pid-tpc-base "${OPTIONS[@]}" | \
o2-analysis-track-propagation "${OPTIONS[@]}" | \
o2-analysis-trackselection "${OPTIONS[@]}" | \
o2-analysis-multiplicity-table "${OPTIONS[@]}" | \
o2-analysis-pid-tpc "${OPTIONS[@]}" | \
o2-analysis-track-to-collision-associator "${OPTIONS[@]}" | \
o2-analysis-hf-track-index-skim-creator "${OPTIONS[@]}" | \
o2-analysis-hf-tree-creator-to-xi-pi "${OPTIONS[@]}" | \
o2-analysis-hf-candidate-creator-xic0-omegac0 "${OPTIONS[@]}" | \
o2-analysis-hf-candidate-selector-to-xi-pi "${OPTIONS[@]}" \
#> "$LOGFILE" 2>&1

# report status
rc=$?
if [ $rc -eq 0 ]; then
  echo "No problems!"
else
  echo "Error: Exit code $rc"
  echo "Check the log file $LOGFILE"
  exit $rc
fi

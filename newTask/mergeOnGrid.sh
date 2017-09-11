#!/bin/bash

# ===========================================================
# Script for local merging & download output on Grid
# ===========================================================

tag=hm-16k
runList=/Users/vpacik/NBI/triggerHMstudies/runlists/LHC16k_temp.runlist

mkdir -pv merge
cd merge

for i in $(cat ${runList})
do
	mkdir -pv merge_${i}
	cd merge_${i}
	path="/alice/cern.ch/user/v/vpacik/${tag}/output/000${i}/"

	root -l -b -q ~/NBI/Flow/macros/mergeOutputOnGrid.C\(\"${path}\"\) &
	cd ../
done

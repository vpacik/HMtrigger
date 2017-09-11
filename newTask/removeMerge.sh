#!/bin/bash

runList=/Users/vpacik/NBI/triggerHMstudies/runlists/LHC16k_temp.runlist

for i in $(cat ${runList})
do
  rm -rfv ./merge/merge_${i}
  # echo "./merge/merge_000${i}"
done

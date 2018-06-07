#!/bin/bash

tag="16l-pass2"

dir="/Users/vpacik/Codes/ALICE/HMtrigger/running/${tag}/"
input="/Users/vpacik/Codes/ALICE/HMtrigger/newTask"

mkdir -pv ${dir}

cp ${input}/AddTaskFilterTrigHMSPD.C ${dir}
cp ${input}/AliAnalysisTaskFilterTrigHMSPD.cxx ${dir}
cp ${input}/AliAnalysisTaskFilterTrigHMSPD.h ${dir}
cp ${input}/runEsd.C ${dir}
cp ${input}/cleanAfterMerge.sh ${dir}
cp ${input}/mergeOnGrid.sh ${dir}
cp ${input}/mergeLocal.sh ${dir}

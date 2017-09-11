#!/bin/bash

dir="./running/15l"


mkdir -pv ${dir}

cp ./AddTaskFilterTrigHMSPD.C ${dir}
cp ./AliAnalysisTaskFilterTrigHMSPD.cxx ${dir}
cp ./AliAnalysisTaskFilterTrigHMSPD.h ${dir}
cp ./runEsd.C ${dir}
cp ./mergeOnGrid.sh ${dir}
cp ./mergeLocal.sh ${dir}

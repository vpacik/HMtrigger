#!/bin/bash

# script for clearing redundant file after running on GRID & after succesfull local merging

rm ./*.xml
rm ./*_cxx.d
rm ./*_cxx.so
rm ./UniFlow*
rm ./myAnalysis.C
rm ./FlowPID*
rm ./gdb-backtrace*
rm ./std*
rm ./outputs_valid

echo "Cleaning done!"

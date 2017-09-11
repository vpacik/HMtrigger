#!/bin/bash

cd ./merge

echo "Checking if local merging result file exits (file: /merge/AnalysisResults.root)"

if [ ! -f ./AnalysisResults.root ]; then
	echo "File NOT found! Merging locally."
	root -l -b -q ~/NBI/Flow/macros/mergeOutput.C
fi

echo "Checking if file exists now"

if [ ! -f ./AnalysisResults.root ]; then
	echo "File NOT found! Something went wrong. EXITING"
	exit
fi

echo "File exists. Cleaning!"

cd ../
mv -v ./merge/AnalysisResults.root ./
#
# if [ -f ./AnalysisResults.root ]; then
# 	rm -rf ./merge
# 	rm ./*.xml
# 	echo "Cleaning succesful!"
# fi

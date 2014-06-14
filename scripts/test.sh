#!/bin/bash

echo "Running test: "
for i in {1..20}
do
  CLIENT_ID=4 ./MovingAverageOneDfe 1> mav.out 2> mav.err &
  
done
echo -n "["
for job in `jobs -p`
do
  echo -n "."
  wait $job || let "FAIL+=1"
done
echo "]"
CLIENT_ID=4 ./TerminateServer

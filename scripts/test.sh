#!/bin/bash

echo "Running test: "
for i in {1..10}
do
  CLIENT_ID=4 ./MovingAverageOneDfe 1> mav.out 2> mav.err &
  sleep 1
done

for job in `jobs -p`
do
  echo $job
  wait $job || let "FAIL+=1"
done

CLIENT_ID=4 ./TerminateServer

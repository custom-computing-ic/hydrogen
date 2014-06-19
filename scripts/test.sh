#!/bin/bash
#../scripts/killprocs.sh
../scripts/startprocs.sh
sleep 0.2
echo "Running test: "
for i in {1..30}
do
   ./MovingAverageOneDfe 1> mav.out 2> mav.err &
done
echo -n "["
for job in `jobs -p`
do
  echo -n "."
  wait $job || let "FAIL+=1"
done
echo "]"

 ./TerminateServer

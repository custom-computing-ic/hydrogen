#!/bin/bash
set -e
./killprocs
sleep 1s
./startprocs




sleep 3s
echo "Running test: "
./MovingAverageOneDfe 1> mav.out 2> mav.err

sleep 3s
echo "Test finished. check mav.out for result"
killall dispatcher &> /dev/null
killall scheduler &> /dev/null
#./MultiUserSingleJobType

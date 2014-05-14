#!/bin/bash

killall dispatcher
killall scheduler
echo "Waiting for any previous processes to release the ports.."
sleep 10s
set -e
./dispatcher 8112 2> dispatcher.err &
./scheduler 8111 8112 2> scheduler.err &


sleep 2s

./MovingAverageOneDfe 2> mav.err

sleep 3s

killall dispatcher &> /dev/null
killall scheduler &> /dev/null
#./MultiUserSingleJobType

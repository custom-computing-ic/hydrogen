#!/bin/bash

set -e
../build/dispatcher 8112 1> dispatcher.out  2> dispatcher.err&
printf "Starting Dispatcher"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"

../build/scheduler 8111 8112 1> scheduler.out  2>dispatcher.err&
printf "Starting Scheduler"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"


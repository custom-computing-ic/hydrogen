#!/bin/bash

set -e
./dispatcher 8112 1> /dev/null &
printf "Starting Dispatcher"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"

../build/scheduler 8111 8112 2> ./log/scheduler.err | grep INFO &
printf "Starting Scheduler"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"
./executor -c 4 -p 8111 -n localhost  1> exe.out 2> exe.err &
printf "Starting Executor"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"
printf "All Processes started. Ready for tests\n"


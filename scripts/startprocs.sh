#!/bin/bash

set -e
../build/dispatcher 8116 1> ./log/dispatcher.out  2> ./log/dispatcher.err&
printf "Starting Dispatcher"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"

../build/scheduler 8115 8116 1> ./log/scheduler.out  2> ./log/scheduler.err&
printf "Starting Scheduler"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"
../build/executor -p 8114 -s 8115 -n localhost 1> ./log/executor.out  2> ./log/executor.err&
printf "Starting Executor"
sleep 1s
printf "."
sleep 1s
printf "."
sleep 1s
printf ".\n"
printf "All Processes started. Ready for tests\n"


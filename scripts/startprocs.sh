#!/bin/bash

set -e
if [ -z "$MAXELEROSDIR" ] || [ -z "$MAXCOMPILERDIR" ]; then
  printf "Starting Dispatcher Without DFE Implementations"
  ./dispatcher 8112 1> /dev/null &
else
  printf "Starting Dispatcher With DFE Implementations"
  LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./dispatcher 8112 1> /dev/null &
fi

sleep 0.2s
printf "."
sleep 0.2s
printf "."
sleep 0.2s
printf ".\n"

../build/scheduler 8111 8112 2> ./log/scheduler.err | grep INFO &
printf "Starting Scheduler"
sleep 0.2s
printf "."
sleep 0.2s
printf "."
sleep 0.2s
printf ".\n"
./executor -c 4 -p 8111 -n localhost  2> ./log/exe.err 1> ./log/exe.out &
printf "Starting Executor"
sleep 0.2s
printf "."
sleep 0.2s
printf "."
sleep 0.2s
printf ".\n"
printf "All Processes started. Ready for tests\n"


#!/bin/bash

set -e
if [ -z "$MAXELEROSDIR" ] || [ -z "$MAXCOMPILERDIR" ]; then
  printf "Starting Dispatcher Without DFE Implementations\n"
  ./dispatcher 8112 1 2 1> /dev/null &
  #TODO: Add cmd line option to force CPU only.
else
  printf "Starting Dispatcher With DFE Implementations\n"
  LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./dispatcher 8112  1 2 1> /dev/null &
fi

sleep 0.2s
printf "."
sleep 0.2s
printf "."
sleep 0.2s
printf ".\n"
printf "Starting Scheduler\n"
../build/scheduler 2> ./log/scheduler.err | grep INFO &
sleep 0.2s
printf "."
sleep 0.2s
printf "."
sleep 0.2s
printf ".\n"
printf "Starting Executor\n"
./executor -c 4 -p 8111 -n localhost  2> ./log/exe.err 1> ./log/exe.out &
sleep 0.2s
printf "."
sleep 0.2s
printf "."
sleep 0.2s
printf ".\n"
printf "All Processes started. Ready for tests\n"


#!/bin/bash

printf "Killing any previous processes...\n"
killall -s KILL dispatcher
killall -s KILL scheduler 
killall -s KILL executor

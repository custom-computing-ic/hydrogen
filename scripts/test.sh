#!/bin/bash

echo "Running test: "
for i in {1..100}
do
  ./MovingAverageOneDfe 1> mav.out 2> mav.err &
done


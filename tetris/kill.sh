#!/bin/bash

kill `cat /home/kotl/kotl-matrix/tetris/pid.txt`
sleep 5
killall tetris_clock
sleep 5
killall -9 tetris_clock
sleep 5

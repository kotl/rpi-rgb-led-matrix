#!/bin/bash

killall tetris_clock
sleep 5
killall -9 tetris_clock
sleep 5

/home/kotl/kotl-matrix/tetris/tetris_clock 200000 20 &
TETRIS_PID=$!

echo $TETRIS_PID > /home/kotl/kotl-matrix/tetris/pid.txt

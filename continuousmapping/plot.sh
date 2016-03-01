#!/usr/bin/gnuplot

unset key

#set term eps
#set output 'path.ps'
set title ''
set border 0
set format x ''
set format y ''
set format z ''
set tics scale 0
set view 60,15

splot 'path.dat' w lp

pause -1 'Hit return to continue'

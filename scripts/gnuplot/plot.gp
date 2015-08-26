set autoscale
unset log
unset label
set xtic auto
set ytic auto
set title "Title"
set xlabel "x"
set ylabel "y"
set nokey
plot "data.dat" with lines
set term eps enhanced color solid
set output "data.eps"
replot
set terminal epslatex color solid
set output "data.tex"
replot
set term x11

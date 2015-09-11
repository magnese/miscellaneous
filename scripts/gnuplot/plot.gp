#!/usr/bin/gnuplot

unset key

set style data lines
set grid ytics
set grid xtics
set term eps enhanced monochrome solid
set output "out.ps"
set title ""
set xlabel ""
set ylabel ""

plot "data.dat"

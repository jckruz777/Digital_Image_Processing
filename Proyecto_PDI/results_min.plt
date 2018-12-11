#!/usr/bin/gnuplot -persist

set title "Min FilterTiming Results"
set xlabel "Kernel Size"
set ylabel "Time (ms)"
set grid
plot "data_min.dat" u (column(0)):2:xtic(1) w l title "LTI-Lib2", "data_min.dat" u (column(0)):3:xtic(1) w l title "OpenCV", "data_min.dat" u (column(0)):4:xtic(1) w l title "Paper", "data_min.dat" u (column(0)):5:xtic(1) w l title "Serial"

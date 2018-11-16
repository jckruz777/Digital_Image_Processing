#!/usr/bin/gnuplot -persist

set title "Gaussian Kernel Timing Results"
set xlabel "Kernel Size"
set ylabel "Time"
set grid
plot "data.dat" u (column(0)):2:xtic(1) w l title "Space-Unified","data.dat" u (column(0)):3:xtic(1) w l title "Space-Sep","data.dat" u (column(0)):4:xtic(1) w l title "Freq-Unified"
set datafile separator ","
set terminal epslatex size 8.5cm, 7.5cm
set output "sentinels.tex"
set size square

set xlabel "Time (days)"
set ylabel "Number of Sentinels"
set style fill transparent solid 0.3 noborder
set style line 1 lc rgb "#56B4E9" lw 2 dt 1
set style line 2 lc rgb "#D55E00" lw 2 dt (6,6)
set style line 3 lc rgb "#00000" lw 2 dt (2,6)

set style line 4 lc rgb "#D55E00" lw 2 dt 1
set style line 5 lc rgb "#00000" lw 2 dt 1

set xtics ("0" 0, "$1000$" 1000, "$2000$" 2000, "$3000$" 3000)
set ytics ("0" 0, "20" 20, "40" 40, "60" 60, "80" 80, "100" 100)
set xrange [0:3000]
set yrange [0:100]
set xtics in
set ytics in

set border back
set pointsize 2

plot \
    "< paste -d, sentinelD_upper.txt sentinelD_lower.txt" using 1:2:4 with filledcurves ls 3 notitle, \
    "< paste -d, sentinelU_upper.txt sentinelU_lower.txt" using 1:2:4 with filledcurves ls 2 notitle, \
    "< paste -d, sentinelH_upper.txt sentinelH_lower.txt" using 1:2:4 with filledcurves ls 1 notitle, \
    "sentinelD_mean.txt" using 1:2 with lines ls 3 notitle, \
    "sentinelD_upper.txt" using 1:2 with lines ls 5 notitle, \
    "sentinelD_lower.txt" using 1:2 with lines ls 5 notitle, \
    "sentinelU_mean.txt" using 1:2 with lines ls 2 notitle, \
    "sentinelU_upper.txt" using 1:2 with lines ls 4 notitle, \
    "sentinelU_lower.txt" using 1:2 with lines ls 4 notitle, \
    "sentinelH_mean.txt" using 1:2 with lines ls 1 notitle, \
    "sentinelH_upper.txt" using 1:2 with lines ls 1 notitle, \
    "sentinelH_lower.txt" using 1:2 with lines ls 1 notitle, \

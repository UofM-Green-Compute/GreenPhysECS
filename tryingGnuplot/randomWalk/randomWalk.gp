set datafile separator ","
set terminal epslatex size 6.7cm, 6.7cm
set output "randomWalk.tex"
set size square

set xlabel "x(m)"
set ylabel "x(m)"

set style line 1 lc rgb "#56B4E9" lw 4 dt 1
set style line 2 lc rgb "#D55E00" lw 4 dt 1

set xtics ("0" 0, "$25$" 25, "$50$" 50)
set ytics ("0" 0, "$25$" 25, "$50$" 50)
set xrange [0:50]
set yrange [0:50]
set xtics in
set ytics in

set border back
set pointsize 2

plot \
    "person1.txt"   using 2:3 with lines ls 1 notitle, \
    "person2.txt"   using 2:3 with lines ls 2 notitle, \
